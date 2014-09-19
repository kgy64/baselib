/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     Android Access Library - an easy to use interface between Java
 *              and native C++ code.
 * Purpose:     Interpret touch-screen events as gestures.
 * Author:      Copyright (c) 2014 - Gyorgy Kovesdi <kgy@teledigit.eu>
 * License:     GPLv2
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __SRC_GESTURE_GESTURE_H_INCLUDED__
#define __SRC_GESTURE_GESTURE_H_INCLUDED__

#include <Threads/Threads.h>
#include <Debug/Debug.h>

#include <Memory/Memory.h>
#include <map>

SYS_DECLARE_MODULE(DM_ANDROID_ACCESS);

class AndroidRenderThread;

namespace AndroidAccess
{
    class Gesture: Threads::Thread
    {
     public:
        Gesture(MEM::shared_ptr<AndroidRenderThread> & parent);
        virtual ~Gesture();

        void TouchData(int32_t index, int64_t time, float x, float y);
        void TouchEvent(int32_t event);

     protected:
        virtual int main(void) override;

        void CreateEvent_1(void);
        void CreateEvent_2(void);
        void Clicked(float x, float y, int number);

        MEM::shared_ptr<AndroidRenderThread> & parent;

        /// Android Touch Event Values
        /*! These are the raw events got from the Android system (see \ref AndroidAccess::Gesture::TouchEvent(int32_t)
         *  for details).<br>
         *  The values are copied from Android Documentation (see the Java class MotionEvent)
         *  \note   The original <a href="http://developer.android.com/reference/android/view/MotionEvent.html">page is here</a>
         */
        enum TouchEvents
        {
            ACTION_DOWN             =   0,
            ACTION_UP               =   1,
            ACTION_MOVE             =   2,
            ACTION_CANCEL           =   3,
            ACTION_OUTSIDE          =   4,
            ACTION_POINTER_DOWN     =   5,
            ACTION_POINTER_UP       =   6,
            ACTION_MASK             =   0x0000ff,
            ACTION_POINTER_ID_MASK  =   0x00ff00,
            ACTION_POINTER_ID_SHIFT =   8,
        };

        struct TouchInfo
        {
            TouchInfo(void):
                x(0.0f),
                y(0.0f)
            {
            }

            float x;

            float y;

        }; // struct AndroidMainService::TouchInfo

        static constexpr int NO_OF_TOUCHPOINTS = 10;

        typedef TouchInfo   Touchs[NO_OF_TOUCHPOINTS];

        typedef std::map<int64_t, Touchs> TouchMap;

        TouchMap touch_data;

        void ExecuteTouchEvent(int event);

        class IState
        {
         public:
            inline IState(Gesture & parent):
                parent(parent),
                start_time(0)
            {
            }

            virtual ~IState()
            {
            }

            virtual void Step(int event) =0;
            virtual bool Timer(void) =0;
            virtual void Elapsed(void) =0;

         protected:
            Gesture & parent;

            int64_t start_time;

         private:
            SYS_DEFINE_CLASS_NAME("AndroidAccess::Gesture::IState");

        }; // class AndroidAccess::Gesture::IState

        typedef MEM::shared_ptr<IState> StatePtr;

        StatePtr state;

        class StateClick: public IState
        {
            static constexpr int RELEASE_TIMEOUT = 6;   // 300 ms

            static constexpr int CLICK_MIN_TIME = 25;   // [ms]

            static constexpr int CLICK_MAX_TIME = 1000; // [ms]

         public:
            StateClick(Gesture & parent):
                IState(parent),
                click_counter(0),
                release_timer(0),
                release_point(RELEASE_TIMEOUT),
                myState(TOUCH_INIT)
            {
                SYS_DEBUG_MEMBER(DM_ANDROID_ACCESS);
            }

            virtual ~StateClick();

         protected:
            enum TouchResults
            {
                TOUCH_INIT  = 0,
                TOUCH_IGNORED,
                TOUCH_CLICKED,
            };

            virtual void Step(int event) override;
            virtual bool Timer(void) override;
            virtual void Elapsed(void) override;

            inline void Reset(void)
            {
                release_point = release_timer + RELEASE_TIMEOUT;
            }

            int click_counter;

            int release_timer;

            int release_point;

            TouchResults myState;

            TouchInfo myPosition;

         private:
            SYS_DEFINE_CLASS_NAME("AndroidAccess::Gesture::StateClick");

        }; // class AndroidAccess::Gesture::StateClick

        friend class StateClick;

        class StateMagnify: public IState
        {
         public:
            StateMagnify(Gesture & parent):
                IState(parent)
            {
                SYS_DEBUG_MEMBER(DM_ANDROID_ACCESS);
            }

            virtual ~StateMagnify()
            {
                SYS_DEBUG_MEMBER(DM_ANDROID_ACCESS);
            }

         protected:
            virtual void Step(int event) override;
            virtual bool Timer(void) override;
            virtual void Elapsed(void) override;

         private:
            SYS_DEFINE_CLASS_NAME("AndroidAccess::Gesture::StateMagnify");

        }; // class AndroidAccess::Gesture::StateMagnify

        friend class StateMagnify;

     private:
        SYS_DEFINE_CLASS_NAME("AndroidAccess::Gesture");

    }; // class AndroidAccess::Gesture

} // namespace AndroidAccess

#endif /* __SRC_GESTURE_GESTURE_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
