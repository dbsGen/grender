//
// Created by gen on 16/5/30.
//

#include "Renderer.h"
#include <utils/NotificationCenter.h>
#include <physics/PhysicsServer.h>
#include <math/Math.hpp>
#include <sys/time.h>
#include "platforms/Platform.h"
#include "object/ui/ImageView.h"
#include "texture/ColorTexture.h"
#include <script/java/JScript.h>

using namespace gr;
using namespace gc;
using namespace std;

const StringName Renderer::NOTIFICATION_PREV_FRAME("NOTIFICATION_RENDER_PREV_FRAME");
const StringName Renderer::NOTIFICATION_POST_FRAME("NOTIFICATION_RENDER_POST_FRAME");
const StringName Renderer::NOTIFICATION_FIXED_UPDATE("NOTIFICATION_RENDER_FIXED_UPDATE");
const StringName Renderer::NOTIFICATION_FRAME_STEP("NOTIFICATION_RENDER_FRAME_STEP");
const StringName Renderer::NOTIFICATION_RENDERER_DELETE("NOTIFICATION_RENDREER_DELETE");
const StringName Renderer::NOTIFICATION_HANDLE_EVENT("NOTIFICATION_RENDER_HANDLE_EVENT");
float Renderer::screen_scale = 1.0;


namespace gr {
    struct WillDoItem {
        void *target;
        ActionItem action;
    };
}

bool Renderer::RenderRoot::travCheck(Object3D *tar, void *data) {
    return tar->getInstanceClass()->isTypeOf(Camera::getClass());
}

void Renderer::RenderRoot::travAddDo(Object3D *tar, void *data) {
    Renderer *renderer = (Renderer*)data;
    renderer->cameras.push_back(tar->cast_to<Camera>());
}

void Renderer::RenderRoot::travRemoveDo(Object3D *tar, void *data) {
    Renderer *renderer = (Renderer*)data;

    Camera *c = tar->cast_to<Camera>();
    for (auto it = renderer->cameras.begin(), _e = renderer->cameras.end(); it != _e; ++it) {
        if (**it == c) {
            renderer->cameras.erase(it);
            break;
        }
    }
}

Renderer::ThreadState Renderer_current_thread = Renderer::SubThread;
mutex Renderer_thread_mtx;

void Renderer::setCurrentThread(bool sub) {
    Renderer_thread_mtx.lock();
    Renderer_current_thread = sub ? Renderer::SubThread : Renderer::MainThread;
    Renderer_thread_mtx.unlock();
}

Renderer::ThreadState Renderer::currentThread() {
    Renderer_thread_mtx.lock();
    Renderer::ThreadState s = Renderer_current_thread;
    Renderer_thread_mtx.unlock();
    return s;
}

bool Renderer::RenderRoot::onMessage(const StringName &key, const RArray *vars) {
    if (key == Object3D::MESSAGE_ADD_CHILD) {
        variant_vector &vec = vars->vec();
        Ref<Object3D> child = vec.front().ref();
//        if (child->getInstanceClass()->isTypeOf(View::getClass()))
//            renderer->getUICamera();
        child->traversal(Renderer::RenderRoot::travCheck, Renderer::RenderRoot::travAddDo, renderer);
        renderer->imp->add(child);
        child->change();
    }else if (key == Object3D::MESSAGE_REMOVE_CHILD) {
        variant_vector &vec = vars->vec();
        Ref<Object3D> child = vec.front().ref();
        child->traversal(Renderer::RenderRoot::travCheck, Renderer::RenderRoot::travRemoveDo, renderer);
        renderer->imp->remove(child);
        child->change();
    }else if (key == Object3D::MESSAGE_ENABLE_CHANGE) {
        Object3D *child = vars->at(0).get<Object3D>();
        bool old = vars->at(1);
        bool n = vars->at(2);
        if (old && !n) {
            child->traversal(Renderer::RenderRoot::travCheck, Renderer::RenderRoot::travRemoveDo, renderer);
            renderer->imp->remove(child);
        }else if (!old && n) {
            child->traversal(Renderer::RenderRoot::travCheck, Renderer::RenderRoot::travAddDo, renderer);
            renderer->imp->add(child);
        }
    } else if (key == Object3D::MESSAGE_CHANGE_MATERIAL) {
        variant_vector &vec = vars->vec();
        Object3D *object = vec.at(0).get<Object3D>();
        Material *mat = vec.at(1).get<Material>();
        renderer->imp->reload(object, mat);
    }else if (key == Object3D::MESSAGE_MASK_CHANGE) {
        variant_vector &vec = vars->vec();
        Ref<Object3D> self((Object3D*)(void*)vec.at(0));
        Mask from = (Mask)(int)vec.at(1);
        Mask to = (Mask)(int)vec.at(2);
        renderer->imp->maskChanged(self, from, to);
    }else if (key == Object3D::MESSAGE_HIT_MASK_CHANGE) {
        variant_vector &vec = vars->vec();
        Object3D *obj = vec.at(0).get<Object3D>();
        Ref<Object3D> self(obj);
        Mask from = (Mask)(int)vec.at(1);
        Mask to = (Mask)(int)vec.at(2);
        renderer->imp->hitMaskChanged(self, from, to);
    }else if (key == Object3D::MESSAGE_DISPLAY_CHANGED) {
        changed();
    }
    return Object3D::onMessage(key, vars);
}

void Renderer::RenderRoot::changed() {
    if (renderer->required_rendering) {
        return;
    }
    renderer->required_reset_count = 4;
    renderer->required_rendering = true;
    if (renderer->rendering)
        renderer->requireRender();
    else renderer->callRender();
}

void Renderer::add(const Ref<Object3D> &object) {
    mtx.lock();
    root.add(object);
    mtx.unlock();
}

void Renderer::remove(const Ref<Object3D> &object)  {
    mtx.lock();
    root.remove(object);
    mtx.unlock();
}

void Renderer::render() {
    render_mtx.lock();
    required_rendering = false;
    imp->prepare();
    setCurrentThread(false);
    will_do_mtx.lock();
    auto do_copy = will_do;
    will_do.clear();
    will_do_mtx.unlock();
    for (auto it = do_copy.begin(), _e = do_copy.end(); it != _e; ++it) {
        WillDoItem *item = (WillDoItem*)*it;
        item->action(this, item->target);
        delete item;
    }
    variant_vector vs1 {this};
    RArray arr(vs1);
    for (auto it = will_callback.begin(), _e = will_callback.end(); it != _e; ++it) {
        (*it)->invoke(arr);
    }
    will_callback.clear();
    for (auto it = next_frame_callback.begin(), _e = next_frame_callback.end(); it != _e; ++it) {
        (*it)->invoke(arr);
    }
    next_frame_callback.clear();
    rendering = true;
    Time current_time = time();
    Time delta = current_time - last_frame;
    NotificationCenter::getInstance()->trigger(NOTIFICATION_PREV_FRAME);
    for (auto it = plugins.begin(), _e = plugins.end(); it != _e; ++it) {
        (*(Ref<Plugin>*)it->second)->step(this, delta);
    }
    vector<Variant> vs{delta};
    NotificationCenter::getInstance()->trigger(NOTIFICATION_FRAME_STEP, &vs);
    last_frame = current_time;
    mtx.lock();
    imp->render();
    mtx.unlock();
    root.updated();
    NotificationCenter::getInstance()->trigger(NOTIFICATION_POST_FRAME);
    rendering = false;
    if (need_render)
        _needUpdate();
    required_rendering = false;
    need_render = false;
    apply("_rendered");
    setCurrentThread(true);
    ++_frame_per_second;
    render_mtx.unlock();
}

void Renderer::step() {
    render_mtx.lock();
    for (auto it = next_frame_callback.begin(), _e = next_frame_callback.end(); it != _e; ++it) {
        (*it)(this);
    }
    next_frame_callback.clear();
    if (required_rendering && --required_reset_count < 0) {
        required_rendering = false;
    }
    Time current_time = time();
    Time delta = current_time - last_step_time;
    for (auto it = plugins.begin(), _e = plugins.end(); it != _e; ++it) {
        (*(Ref<Plugin>*)it->second)->fixedStep(this, delta);
    }
    vector<Variant> vs{delta};
    NotificationCenter::getInstance()->trigger(NOTIFICATION_FIXED_UPDATE, &vs);
    last_step_time = current_time;
    apply("_fixedUpdate");

    Time t = time();
    if (t > last_count_time + 1) {
        frame_per_second = _frame_per_second;
        last_count_time = t;
        _frame_per_second = 0;
        LOG(i, "fps:%d dc:%d", frame_per_second, draw_call_count);
    }

    render_mtx.unlock();
}

void Renderer::callRender() {
    _requireRender();
    apply("_requireRender");
}

void Renderer::fixedThreadStart(void *loop_thread, void *_, void *that) {
#ifdef __ANDROID__
    gscript::JScript::registerThread();
#endif
}

void Renderer::fixedThreadOver(void *loop_thread, void *_, void *that) {
#ifdef __ANDROID__
    gscript::JScript::unregisterThread();
#endif
}

void Renderer::fixedProcess(void *loop_thread, void *_, void *that) {
    Renderer *renderer = (Renderer *)that;
    renderer->step();
    Time last = renderer->last_step_time;
    Time t = time() - last;
    if (t < renderer->fixed_interval) {
        Time w = renderer->fixed_interval - t;
        static struct timespec wait;
        wait.tv_sec = (int)w;
        wait.tv_nsec = (w - wait.tv_sec)*1E9;
        nanosleep(&wait, NULL);
    }
}

Renderer::Renderer() : size(2,2),
                       need_render(false),
                       rendering(false),
                       fixed_thread(fixedProcess, this),
                       dynamic_frame(false),
                       draw_call_count(0),
                       frame_per_second(0),
                       _draw_call_count(0),
                       _frame_per_second(0),
                       last_count_time(0),
                       required_reset_count(0) {
    required_rendering = false;
    setFixedRate(60);
    imp = (RendererIMP *)gg::Factory::getInstance()->create(RendererIMP::getClass());
    imp->_setTarget(this);
    root.renderer = this;
    last_frame = time();

    if(shared_instance == NULL){
        shared_instance = this;
    }
    fixed_thread.setOnStart(&Renderer::fixedThreadStart, this);
    fixed_thread.setOnOver(&Renderer::fixedThreadOver, this);
    fixed_thread.start();
}
Renderer* Renderer::shared_instance = NULL;

Renderer *Renderer::sharedInstance() {
    return shared_instance;
}

Renderer::~Renderer() {
    //fixed_thread.cancel();
    setMainController(NULL);
//    PhysicsServer::getInstance()->clear();
    setCurrentThread(true);
    for (auto it = next_frame_callback.begin(), _e = next_frame_callback.end(); it != _e; ++it) {
        (*it)(this);
    }
    delete imp;
    for (auto it = will_do.begin(), _e = will_do.end(); it != _e; ++it) {
        delete (WillDoItem*)*it;
    }
}

void *Renderer::doOnMainThread(void *target, ActionCallback callback, void *data) {
    void *ret = NULL;
    if (currentThread() == Renderer::SubThread) {
        will_do_mtx.lock();
        for (auto it = will_do.begin(), _e = will_do.end(); it != _e; ++it) {
            WillDoItem *item = (WillDoItem *)*it;
            if (item->target == target) {
                ret = item->action.data;
                delete item;
                will_do.erase(it);
                break;
            }
        }
        WillDoItem *item = new WillDoItem;
        item->target = target;
        item->action.callback = callback;
        item->action.data = data;
        will_do.push_back(item);
        will_do_mtx.unlock();
    }else {
        callback(this, target, data);
    }
    return ret;
}

void *Renderer::cancelDoOnMainThread(void *target) {
    will_do_mtx.lock();
    for (auto it = will_do.begin(), _e = will_do.end(); it != _e; ++it) {
        WillDoItem *item = (WillDoItem *)*it;
        if (item->target == target) {
            void *res = item->action.data;
            will_do.erase(it);
            will_do_mtx.unlock();
            delete item;
            return res;
        }
    }
    will_do_mtx.unlock();
    return NULL;
}

void Renderer::setSize(const Size &size) {
    this->size = size;
    imp->updateSize(size);
    if (main_controller) {
        main_controller->onResize(size);
    }
}

void Renderer::destroy(Renderer *renderer) {
    renderer->fixed_thread.cancel();
    NotificationCenter::getInstance()->trigger(NOTIFICATION_RENDERER_DELETE);
    renderer->setMainController(NULL);
    if (shared_instance == renderer) shared_instance = NULL;
    for (auto it = renderer->plugins.begin(); it != renderer->plugins.end(); ++it) {
        delete (Ref<Plugin>*)it->second;
    }

    thread t(&Renderer::_destroy, renderer);
    t.detach();
}

void Renderer::_destroy(Renderer *renderer) {
    static struct timespec wait;
    wait.tv_sec = 0;
    wait.tv_nsec = 10;
    nanosleep(&wait, NULL);
    delete renderer;
}

//void Renderer::touchBegin(const Vector2f &point) {
//    const Ref<Camera> &camera = getUICamera();
////    const Rect &rect = camera->getRect();
////    float offx = (point.x() / rect.v[2])*2 - (rect.v[2]/fabsf(rect.v[2]));
////    float offy = (point.y() / rect.v[3])*2 - (rect.v[3]/fabsf(rect.v[3]));
////    Point original = camera->screenToWorld(Point(offx, offy, 0));
////    Point direction = camera->screenToWorld(Point(offx, offy, 1/(camera->getFar() - camera->getNear())));
////    Ray ray(original, direction - original);
////    ray.setHitMask(camera->getHitMask());
//    Vector2f vp = Vector2f(point.x()/size.x(), point.y()/size.y());
//    if (camera->getEnable()) {
//        camera->sendTouchEvent(Object::TOUCH_BEGIN, vp);
//    }
//    vector<Variant> vs{
//        Variant(Object::TOUCH_BEGIN),
//        vp
//    };
//    NotificationCenter::getInstance()->trigger(NOTIFICATION_HANDLE_EVENT, &vs);
//}
//
//void Renderer::touchMove(const Vector2f &point) {
//    const Ref<Camera> &camera = getUICamera();
//    Vector2f vp = Vector2f(point.x()/size.x(), point.y()/size.y());
//    if (camera->getEnable()) {
//        camera->sendTouchEvent(Object::TOUCH_MOVE, vp);
//    }
//    vector<Variant> vs{
//        Variant(Object::TOUCH_MOVE),
//        vp
//    };
//    NotificationCenter::getInstance()->trigger(NOTIFICATION_HANDLE_EVENT, &vs);
//}
//
//void Renderer::touchEnd(const Vector2f &point) {
//    const Ref<Camera> &camera = getUICamera();
//    Vector2f vp = Vector2f(point.x()/size.x(), point.y()/size.y());
//    if (camera->getEnable()) {
//        camera->sendTouchEvent(Object::TOUCH_END, vp);
//    }
//    vector<Variant> vs{
//        Variant(Object::TOUCH_END),
//        vp
//    };
//    NotificationCenter::getInstance()->trigger(NOTIFICATION_HANDLE_EVENT, &vs);
//}
//
//void Renderer::touchCancel(const Vector2f &point) {
//    const Ref<Camera> &camera = getUICamera();
//    Vector2f vp = Vector2f(point.x()/size.x(), point.y()/size.y());
//    if (camera->getEnable()) {
//        camera->sendTouchEvent(Object::TOUCH_CANCEL, vp);
//    }
//    vector<Variant> vs{
//        Variant(Object::TOUCH_CANCEL),
//        vp
//    };
//    NotificationCenter::getInstance()->trigger(NOTIFICATION_HANDLE_EVENT, &vs);
//}

void Renderer::setMainController(const Ref<Controller> &controller) {
    if (main_controller != controller) {
        if (main_controller) {
            main_controller->willDisappear();
            remove(main_controller->getMainObject());
            main_controller->unload();
            main_controller->afterDisappear();
            main_controller->disattach(this);
        }
        Reference *ref = new Reference(main_controller);
        doNextFrame(C([=](){
            delete ref;
        }));
        main_controller = controller;
        if (main_controller) {
            main_controller->willAppear();
            main_controller->attach(this);
            add(main_controller->getMainObject());
            main_controller->afterDisappear();
        }
    }
}

const Ref<Controller> &Renderer::getMainController() {
    return main_controller;
}

void Renderer::attach(const Ref<Plugin> &plugin) {
    plugins[plugin->getName()] = new Ref<Plugin>(plugin);
    plugin->attach(this);
    plugin->renderer = this;
}

const Ref<Plugin> &Renderer::plugin(const StringName &name) {
    auto it = plugins.find(name);
    if (it != plugins.end()) {
        return *(Ref<Plugin>*)it->second;
    }
    return Ref<Plugin>::null();
}

void Renderer::disattach(const StringName &name) {
    auto it = plugins.find(name);
    if (it != plugins.end()) {
        Ref<Plugin> *p = (Ref<Plugin>*)it->second;
        (*p)->disattach(this);
        delete p;
        plugins.erase(it);
    }
}

const Ref<Camera> &Renderer::getCamera(int index) {
    return cameras[index];
}

void Renderer::requireRender() {
    if (rendering)
        need_render = true;
    else
        _needUpdate();
}

Time Renderer::time() {
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return tp.tv_sec + tp.tv_usec * 1.0E-6;
}

void Renderer::reload(Object3D *object) {
    imp->reload(object, NULL);
}

void Renderer::maskChange(Object3D *object, Mask from, Mask to) {
    imp->maskChanged(object, from, to);
}

void Renderer::hitMaskChange(Object3D *object, Mask from, Mask to) {
    imp->hitMaskChanged(object, from, to);
}
