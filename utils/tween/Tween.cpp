//
// Created by gen on 16/7/10.
//

#include <Renderer.h>
#include "Tween.h"
#include "utils/NotificationCenter.h"
#include <core/math/Math.hpp>

using namespace gr;

const StringName TweenManager::NOTIFICATION_KEY("TweenManager_FrameStep");

Tween::ProcessResult Tween::update(double delta) {
    if(!object) return Complete;
    bool check = false, is_forword;
    if (status == PlayForword) {
        Time oldTime = time_left;
        time_left += delta;
        if (time_left >= duration + delay) {
            check = true;
            time_left = duration + delay;
        }
        for (int i = 0, t = events.size(); i < t; ++i) {
            for (auto it = events.items.begin(), _e = events.items.end(); it != _e; ++it) {
                const ActionItem &item = *it;
                Time time = (long)item.data * 1E-6;
                if ((time == 0 && oldTime == 0) || (time > oldTime && time <= time_left)) {
                    item(this);
                }
            }
        }
        is_forword = true;
    }else if (status == PlayBackword) {
        Time oldTime = time_left;
        time_left -= delta;
        if (time_left <= 0) {
            check = true;
        }
        for (int i = 0, t = events.size(); i < t; ++i) {
            for (auto it = events.items.begin(), _e = events.items.end(); it != _e; ++it) {
                const ActionItem &item = *it;
                Time time = (long)item.data * 1E-6;
                if ((time == 0 && oldTime == 0) || (time > oldTime && time <= time_left)) {
                    item(this);
                }
            }
        }
        is_forword = false;
    }else {
        return Delay;
    }
    if (check) {
        double p = ease->interpolation(is_forword ? 1 : 0);
        blocks(this, &p);
        
        for (auto it = properties.begin(), _e = properties.end(); it != _e; ++it) {
            ((TweenProperty*)(**it))->process(object, p);
        }
        on_update(this);
        if (loop) {
            on_loop(this);
            time_left = is_forword ? 0 : delay + duration;
            initializeTween(is_forword);
            return Running;
        }else {
            on_complete(this);
            status = Stop;
            return Complete;
        }
    }else {
        double p = ease->interpolation(hi_max((time_left - delay)/(double)duration, (double)0));
        blocks(this, &p);
        for (auto it = properties.begin(), _e = properties.end(); it != _e; ++it) {
            ((TweenProperty*)(**it))->process(object, p);
        }
        on_update(this, object);
        return Delay;
    }
}

Tween::Tween(HObject *target, Time duration, Ease *ease) : Tween() {
    object = target;
    if (target) {
        target->pushOnDestroy(&Tween::objectDestroy, this);
    }
    this->duration = duration;
    this->ease = ease;
}

void Tween::objectDestroy(void *sender, void *send_data, void *data) {
    Tween *tween = (Tween*)data;
    tween->object = NULL;
    tween->stop();
}

void Tween::start() {
    if (status == NotStart) {
        initializeTween(true);
        status = PlayForword;
        Ref<TweenManager> tm = TweenManager::sharedInstance();
        if (tm) tm->add(this);
    }
}

void Tween::pause() {
    if (status == PlayForword ||
            status == PlayBackword) {
        status = Pause;
        const Ref<TweenManager> &tm = TweenManager::sharedInstance();
        if (tm) tm->remove(this);
    }
}

void Tween::stop() {
    if (status == PlayForword ||
            status == PlayBackword ||
            status == Pause) {
        const Ref<TweenManager> &tm = TweenManager::sharedInstance();
        if (tm) tm->remove(this);
    }
}

void Tween::reset() {
    status = NotStart;
    const Ref<TweenManager> &tm = TweenManager::sharedInstance();
    if (tm) tm->remove(this);

    double p = ease->interpolation(0);
    blocks(this, &p);
}

void Tween::backword() {
    switch (status) {
        case NotStart: {
            initializeTween(false);
            status = PlayBackword;
            const Ref<TweenManager> &tm = TweenManager::sharedInstance();
            if (tm) tm->add(this);
            break;
        }
        case Pause: {
            status = PlayBackword;
            const Ref<TweenManager> &tm = TweenManager::sharedInstance();
            if (tm) tm->add(this);
        }
        case PlayForword: {
            status = PlayBackword;
        }
        default:
            break;
    }
}

void Tween::cancel(HObject *target) {
    TweenManager *manager = TweenManager::sharedInstance();
    if (manager) {
        const list<Ref<Tween> > &tweens = manager->getTweens();
        auto it = tweens.begin(), _e = tweens.end();
        while (it != _e) {
            if ((*it)->getTarget() == target)
                (*it)->_cancel = true;
            ++it;
        }
    }
}

void Tween::add(ActionCallback handle, void *data) {
    blocks.push(handle, data);
}

void Tween::clear() {
    blocks.clear();
}

void Tween::addProperty(const Ref<TweenProperty> &property) {
    properties.push_back(property);
}

void Tween::clearProperties() {
    properties.clear();
}

Tween::~Tween() {
    if (object) object->removeOnDestroy(&Tween::objectDestroy, this);
    TweenManager *tm = TweenManager::sharedInstance();
    if (tm) tm->remove(this);
    if (ease) delete ease;
}

Tween::Tween(): object(NULL),
                delay(0),
                time_left(0),
                duration(200),
                loop(false),
                over(false),
                status(NotStart),
                _cancel(false),
                ease(NULL) {

}

void Tween::addListener(Time time, ActionCallback handle) {
    long t = time * 1E6;
    events.push(handle, (void*)t);
}

void Tween::clearListener() {
    events.clear();
}


double QuadraticIn::interpolation(double p) {
    return p*p;
}

double QuadraticOut::interpolation(double p) {
    return p * ( 2 - p );
}

double QuadraticInOut::interpolation(double p) {
    if ( ( p *= 2 ) < 1 ) return 0.5 * p * p;
    p-=1;
    return - 0.5 * ( p * ( p - 2 ) - 1 );
}

double CubicIn::interpolation(double p) {
    return p*p*p;
}

double CubicOut::interpolation(double p) {
    p -=1;
    return p * p * p + 1;
}

double CubicInOut::interpolation(double p) {
    if ( ( p *= 2 ) < 1 ) return 0.5 * p * p * p;
    p -= 2;
    return 0.5 * ( p * p * p + 2 );
}

double QuarticIn::interpolation(double p) {
    return p * p * p * p;
}
double QuarticOut::interpolation(double p) {
    p-=1;
    return 1 - ( p * p * p * p );
}
double QuarticInOut::interpolation(double p) {
    if ( ( p *= 2 ) < 1) return 0.5 * p * p * p * p;
    p -= 2;
    return - 0.5 * ( p * p * p * p - 2 );
}

double SinusoidalIn::interpolation(double p) {
    return 1 - cosf( p * M_PI / 2 );
}
double SinusoidalOut::interpolation(double p) {
    return sinf( p * M_PI / 2 );
}
double SinusoidalInOut::interpolation(double p) {
    return 0.5 * ( 1 - cosf( M_PI * p ) );
}

double ExponentialIn::interpolation(double p) {
    return p == 0 ? 0 : powf( 1024, p - 1 );
}
double ExponentialOut::interpolation(double p) {
    return p == 1 ? 1 : 1 - powf( 2, - 10 * p );
}
double ExponentialInOut::interpolation(double p) {
    if ( p == 0 ) return 0;
    if ( p == 1 ) return 1;
    if ( ( p *= 2 ) < 1 ) return 0.5 * powf( 1024, p - 1 );
    return 0.5 * ( - powf( 2, - 10 * ( p - 1 ) ) + 2 );
}

double CircularIn::interpolation(double p) {
    return 1 - sqrtf( 1 - p * p );
}
double CircularOut::interpolation(double p) {
    p-=1;
    return sqrtf( 1 - ( p * p ) );
}
double CircularInOut::interpolation(double p) {
    if ( ( p *= 2 ) < 1) return - 0.5 * ( sqrtf( 1 - p * p) - 1);
    p -= 2;
    return 0.5 * ( sqrtf( 1 - p * p) + 1);
}

double ElasticIn::interpolation(double k) {
    float s, a = 0.1, p = 0.4;
    if ( k == 0 ) return 0;
    if ( k == 1 ) return 1;
    if ( !a || a < 1 ) { a = 1; s = p / 4; }
    else s = p * asinf( 1 / a ) / ( 2 * M_PI );
    k -= 1;
    return - ( a * powf( 2, 10 * k ) * sinf( ( k - s ) * ( 2 * M_PI ) / p ) );
}
double ElasticOut::interpolation(double k) {
    float s, a = 0.1, p = 0.4;
    if ( k == 0 ) return 0;
    if ( k == 1 ) return 1;
    if ( !a || a < 1 ) { a = 1; s = p / 4; }
    else s = p * asinf( 1 / a ) / ( 2 * M_PI );
    return ( a * powf( 2, - 10 * k) * sinf( ( k - s ) * ( 2 * M_PI ) / p ) + 1 );
}
double ElasticInOut::interpolation(double k) {
    float s, a = 0.1, p = 0.4;
    if ( k == 0 ) return 0;
    if ( k == 1 ) return 1;
    if ( !a || a < 1 ) { a = 1; s = p / 4; }
    else s = p * asinf( 1 / a ) / ( 2 * M_PI );
    if ( ( k *= 2 ) < 1 ) return - 0.5 * ( a * powf( 2, 10 * ( k -= 1 ) ) * sinf( ( k - s ) * ( 2 * M_PI ) / p ) );
    return a * powf( 2, -10 * ( k -= 1 ) ) * sinf( ( k - s ) * ( 2 * M_PI ) / p ) * 0.5 + 1;
}

double BackIn::interpolation(double k) {
    float s = 1.70158;
    return k * k * ( ( s + 1 ) * k - s );
}
double BackOut::interpolation(double k) {
    float s = 1.70158;
    k-=1;
    return k * k * ( ( s + 1 ) * k + s ) + 1;
}
double BackInOut::interpolation(double k) {
    float s = 1.70158 * 1.525;
    if ( ( k *= 2 ) < 1 ) return 0.5 * ( k * k * ( ( s + 1 ) * k - s ) );
    k -= 2;
    return 0.5 * ( k * k * ( ( s + 1 ) * k + s ) + 2 );
}

double BounceIn::interpolationS(double p) {
    return 1 - BounceOut::interpolationS(p);
}
double BounceIn::interpolation(double p) {
    return interpolationS(p);
}
double BounceOut::interpolationS(double k) {
    if ( k < ( 1 / 2.75 ) ) {
        return 7.5625 * k * k;

    } else if ( k < ( 2 / 2.75 ) ) {
        k -= ( 1.5 / 2.75 );
        return 7.5625 * k * k + 0.75;

    } else if ( k < ( 2.5 / 2.75 ) ) {
        k -= ( 2.25 / 2.75 );
        return 7.5625 * k * k + 0.9375;
    } else {
        k -= ( 2.625 / 2.75 );
        return 7.5625 * k * k + 0.984375;
    }
}
double BounceOut::interpolation(double k) {
    return interpolationS(k);
}
double BounceInOut::interpolation(double p) {
    if ( p < 0.5 ) return BounceIn::interpolationS(p * 2) * 0.5;
    return BounceOut::interpolationS(p*2 - 1) * 0.5 + 0.5;
}

double QuinticIn::interpolation(double k) {
    return k * k * k * k * k;
}
double QuinticOut::interpolation(double k) {
    k-=1;
    return k * k * k * k * k + 1;
}
double QuinticInOut::interpolation(double k) {
    if ( ( k *= 2 ) < 1 ) return 0.5 * k * k * k * k * k;
    k -= 2;
    return 0.5 * ( k * k * k * k * k + 2 );
}

TweenManager *TweenManager::instance;
mutex TweenManager::share_mtx;

void TweenManager::step(Renderer *renderer, Time delta) {
    if (!renderer->isDynamicFrame()) {
        mtx.lock();
        auto arr = tweens;
        mtx.unlock();
        if (arr.size()) {
            for (auto it = arr.begin(), _e = arr.end(); it != _e; ++it) {
                Ref<Tween> ref = (*it);
                if (ref->_cancel || ref->update(delta) == Tween::Complete)
                    tweens.erase(LIST_SEARCH(tweens, ref));
            }
        }
    }
}

void TweenManager::fixedStep(Renderer *renderer, Time delta) {
    if (renderer->isDynamicFrame()) {
        mtx.lock();
        auto arr = tweens;
        mtx.unlock();
        if (arr.size()) {
            auto arr = tweens;
            for (auto it = arr.begin(), _e = arr.end(); it != _e; ++it) {
                Ref<Tween> ref = (*it);
                if (ref->_cancel || ref->update(delta) == Tween::Complete)
                    tweens.remove(ref);
            }
        }
    }
}

void TweenManager::add(const Ref<Tween> &tween) {
    mtx.lock();
    tweens.push_back(tween);
    mtx.unlock();
}

void TweenManager::clear() {
    mtx.lock();
    tweens.clear();
    mtx.unlock();
}

TweenManager::~TweenManager() {
    if (instance == this)
        instance = NULL;
}

TweenManager::TweenManager() {
    instance = this;
}

TweenManager *TweenManager::sharedInstance() {
    share_mtx.lock();
    TweenManager *manager = instance;
    share_mtx.unlock();
    return manager;
}
