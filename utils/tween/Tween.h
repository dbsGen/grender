//
// Created by gen on 16/7/10.
//

#ifndef VOIPPROJECT_TWEEN_H
#define VOIPPROJECT_TWEEN_H

#include <core/Singleton.h>
#include <Plugin.h>
#include <core/Action.h>
#include "utils/NotificationCenter.h"

#include <render_define.h>

using namespace gcore;

namespace gr {

    typedef double Time;
    class TweenManager;

    CLASS_BEGIN_0_N(Ease)

    public:
        virtual double interpolation(double p) {return p;}

    CLASS_END

    CLASS_BEGIN_N(QuadraticIn, Ease)
    public:
        virtual double interpolation(double p);
    CLASS_END
    CLASS_BEGIN_N(QuadraticOut, Ease)
    public:
        virtual double interpolation(double p);
    CLASS_END
    CLASS_BEGIN_N(QuadraticInOut, Ease)
    public:
        virtual double interpolation(double p);
    CLASS_END
    CLASS_BEGIN_N(CubicIn, Ease)
    public:
        virtual double interpolation(double p);
    CLASS_END
    CLASS_BEGIN_N(CubicOut, Ease)
    public:
        virtual double interpolation(double p);
    CLASS_END
    CLASS_BEGIN_N(CubicInOut, Ease)
    public:
        virtual double interpolation(double p);
    CLASS_END
    CLASS_BEGIN_N(QuarticIn, Ease)
    public:
        virtual double interpolation(double p);
    CLASS_END
    CLASS_BEGIN_N(QuarticOut, Ease)
    public:
        virtual double interpolation(double p);
    CLASS_END
    CLASS_BEGIN_N(QuarticInOut, Ease)
    public:
        virtual double interpolation(double p);
    CLASS_END
    CLASS_BEGIN_N(QuinticIn, Ease)
    public:
        virtual double interpolation(double p);
    CLASS_END
    CLASS_BEGIN_N(QuinticOut, Ease)
    public:
        virtual double interpolation(double p);
    CLASS_END
    CLASS_BEGIN_N(QuinticInOut, Ease)
    public:
        virtual double interpolation(double p);
    CLASS_END
    CLASS_BEGIN_N(SinusoidalIn, Ease)
    public:
        virtual double interpolation(double p);
    CLASS_END
    CLASS_BEGIN_N(SinusoidalOut, Ease)
    public:
        virtual double interpolation(double p);
    CLASS_END
    CLASS_BEGIN_N(SinusoidalInOut, Ease)
    public:
        virtual double interpolation(double p);
    CLASS_END
    CLASS_BEGIN_N(ExponentialIn, Ease)
    public:
        virtual double interpolation(double p);
    CLASS_END
    CLASS_BEGIN_N(ExponentialOut, Ease)
    public:
        virtual double interpolation(double p);
    CLASS_END
    CLASS_BEGIN_N(ExponentialInOut, Ease)
    public:
        virtual double interpolation(double p);
    CLASS_END
    CLASS_BEGIN_N(CircularIn, Ease)
    public:
        virtual double interpolation(double p);
    CLASS_END
    CLASS_BEGIN_N(CircularOut, Ease)
    public:
        virtual double interpolation(double p);
    CLASS_END
    CLASS_BEGIN_N(CircularInOut, Ease)
    public:
        virtual double interpolation(double p);
    CLASS_END
    CLASS_BEGIN_N(ElasticIn, Ease)
    public:
        virtual double interpolation(double p);
    CLASS_END
    CLASS_BEGIN_N(ElasticOut, Ease)
    public:
        virtual double interpolation(double p);
    CLASS_END
    CLASS_BEGIN_N(ElasticInOut, Ease)
    public:
        virtual double interpolation(double p);
    CLASS_END
    CLASS_BEGIN_N(BackIn, Ease)
    public:
        virtual double interpolation(double p);
    CLASS_END
    CLASS_BEGIN_N(BackOut, Ease)
    public:
        virtual double interpolation(double p);
    CLASS_END
    CLASS_BEGIN_N(BackInOut, Ease)
    public:
        virtual double interpolation(double p);
    CLASS_END
    CLASS_BEGIN_N(BounceIn, Ease)
    public:
        static double interpolationS(double p);
        virtual double interpolation(double p);
    CLASS_END
    CLASS_BEGIN_N(BounceOut, Ease)
    public:
        static double interpolationS(double p);
        virtual double interpolation(double p);
    CLASS_END
    CLASS_BEGIN_N(BounceInOut, Ease)
    public:
        virtual double interpolation(double p);
    CLASS_END

    class Tween;

    CLASS_BEGIN_0_NV(TweenProperty)

    public:
        virtual void process(HObject *target, double p) = 0;

    CLASS_END

    CLASS_BEGIN_N(Tween, RefObject)

    public:
        enum Status {
            NotStart,
            PlayForword,
            PlayBackword,
            Pause,
            Stop
        };

        enum ProcessResult {
            Delay,
            Running,
            Complete
        };

    private:
        ActionManager events;
        HObject *object;
        ActionManager blocks;
        ref_vector properties;
        Ease *ease;

        Time duration;
        Time delay;
        Time time_left;

        bool _cancel;
        bool loop;
        bool over;
        Status status;

        ActionItem on_update;
        ActionItem on_complete;
        ActionItem on_loop;
        static void objectDestroy(void *sender, void *send_data, void *data);

        Tween(HObject *target, Time duration, Ease *ease);

        friend class TweenManager;

    public:
        _FORCE_INLINE_ const HObject *getTarget() {return object;}
        _FORCE_INLINE_ float getCurrentPercent() {return (time_left - delay)/(float)duration;}
        _FORCE_INLINE_ bool isOver() {return over;}
        _FORCE_INLINE_ const Ease *getEase() { return ease; }
        _FORCE_INLINE_ Status getStatus() { return status; }
        _FORCE_INLINE_ Time getDuration() { return duration; }
        _FORCE_INLINE_ Time getDelay() { return delay; }
        _FORCE_INLINE_ void setDelay(Time delay) { this->delay = delay; }
        _FORCE_INLINE_ bool isLoop() {return loop; }
        _FORCE_INLINE_ void setLoop(bool loop) {this->loop = loop;}

        _FORCE_INLINE_ void setOnUpdate(ActionCallback callback, void *data = NULL) {
            on_update.callback = callback;
            on_update.data = data;
        }
        _FORCE_INLINE_ void setOnComplete(ActionCallback callback, void *data = NULL) {
            on_complete.callback = callback;
            on_complete.data = data;
        }
        _FORCE_INLINE_ void setOnLoop(ActionCallback callback, void *data = NULL) {
            on_loop.callback = callback;
            on_loop.data = data;
        }

        ProcessResult update(double delta);

        void start();
        void pause();
        static void cancel(HObject *target);
        void stop();
        void reset();
        void backword();

        void add(ActionCallback handle, void *data = NULL);
        void clear();

        void addProperty(const Ref<TweenProperty> &property);
        void clearProperties();

        void addListener(Time time, ActionCallback handle);
        void clearListener();

        virtual void initializeTween(bool forword) {}

        Tween();
        template <class T=Ease>
        _FORCE_INLINE_ static Tween *New(HObject *target, Time duration) {
            return new Tween(target, duration, new T());
        }

        ~Tween();

    CLASS_END

    CLASS_BEGIN_N(TweenManager, Plugin)

    private:
        static const StringName NOTIFICATION_KEY;

        list<Ref<Tween> > tweens;
        static TweenManager *instance;
        static mutex share_mtx;
        mutex mtx;

        friend class Tween;

    public:
        const list<Ref<Tween> > &getTweens() {return tweens;}

        TweenManager();
        ~TweenManager();

        static TweenManager *sharedInstance();

        virtual void step(Renderer *renderer, Time delta);
        virtual void fixedStep(Renderer *renderer, Time delta);

        void add(const Ref<Tween> &tween);
        void clear();
        void remove(Tween *tween) {
            for (auto it = tweens.begin(), _e = tweens.end(); it != _e ; ++it) {
                if (**it == tween) {
                    tweens.erase(it);
                    return;
                }
            }
        }

    CLASS_END
}


#endif //VOIPPROJECT_TWEEN_H
