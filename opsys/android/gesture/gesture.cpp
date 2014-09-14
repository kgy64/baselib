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

#include "gesture.h"

#include <android-render.h>

using namespace AndroidAccess;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                                       *
 *       class Gesture:                                                                  *
 *                                                                                       *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

Gesture::Gesture(boost::shared_ptr<AndroidRenderThread> & parent):
    parent(parent)
{
 SYS_DEBUG_MEMBER(DM_ANDROID_ACCESS);

 // This thread is used only for periodically call the actual gesture handler class, so
 // 64k must be enough for stack:
 Start(0x10000);
}

Gesture::~Gesture()
{
 SYS_DEBUG_MEMBER(DM_ANDROID_ACCESS);
}

int Gesture::main(void)
{
 SYS_DEBUG_MEMBER(DM_ANDROID_ACCESS);

 try {
    while (!IsFinished()) {
        usleep(50000);      // fixed 20 Hz (50 ms)
        StatePtr s = state; // copy it to prevent race condition
        if (s) {
            if (s->Timer()) {
                state->Elapsed();
                state.reset();
            }
        }
    }
 } catch (EX::Assert & ex) {
    DEBUG_OUT("Assert failed in Gesture: " << ex.what());
 } catch (std::exception & ex) {
    DEBUG_OUT("Exception caught in Gesture: " << ex.what());
 } catch (...) {
    DEBUG_OUT("Unknown exception in Gesture");
 }
 DEBUG_OUT("Gesture::main() exited.");
}

void Gesture::Clicked(float x, float y, int number)
{
 SYS_DEBUG_MEMBER(DM_ANDROID_ACCESS);
 SYS_DEBUG(DL_INFO2, "Clicked(" << x << ", " << y << ", " << number << ")");

 boost::shared_ptr<AndroidRenderThread> render = parent;

 if (!render) {
    return;
 }

 render->MouseClick(x, y, 0, number);
 SYS_DEBUG(DL_INFO2, "Click OK.");
}

void Gesture::TouchData(int32_t index, int64_t time, float x, float y)
{
 SYS_DEBUG_MEMBER(DM_ANDROID_ACCESS);
 SYS_DEBUG(DL_INFO2, "Index=" << index << ", time=" << time << ", pos: " << x << "x" << y);

 if (index < 0 || index >= NO_OF_TOUCHPOINTS) {
    DEBUG_OUT("Warning: touchpoint index " << index << " out of range!");
    return;
 }

 TouchInfo & info = touch_data[time][index];
 info.x = x;
 info.y = y;
}

void Gesture::TouchEvent(int32_t event)
{
 SYS_DEBUG_MEMBER(DM_ANDROID_ACCESS);
 SYS_DEBUG(DL_INFO2, "Event: " << event);

 if (event < 0) {
    touch_data.clear();
 } else {
    ExecuteTouchEvent(event);
 }
}

void Gesture::ExecuteTouchEvent(int event)
{
 SYS_DEBUG_MEMBER(DM_ANDROID_ACCESS);

 SYS_DEBUG(DL_INFO2, "Event: " << event);

 int action = event & ACTION_MASK;

 StatePtr s = state;

 switch (action) {
    case ACTION_DOWN:
        // This is always the first step:
        if (!s) {
            CreateEvent_1();
        }
    break;
 }

 if (!s) {
    s = state;
    if (!s) {
        DEBUG_OUT("Warning: Event called without handler: " << action);
        return;
    }
 }

 s->Step(event);
}

void Gesture::CreateEvent_1(void)
{
 SYS_DEBUG_MEMBER(DM_ANDROID_ACCESS);
 state.reset(new StateClick(*this));
}

void Gesture::CreateEvent_2(void)
{
 SYS_DEBUG_MEMBER(DM_ANDROID_ACCESS);
 state.reset(new StateMagnify(*this));
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                                       *
 *       class Gesture::StateClick:                                                      *
 *                                                                                       *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

Gesture::StateClick::~StateClick()
{
 SYS_DEBUG_MEMBER(DM_ANDROID_ACCESS);
}

bool Gesture::StateClick::Timer(void)
{
 DEBUG_OUT("Timer is " << release_timer << "-" << release_point);
 return (++release_timer - release_point) >= 0;
}

void Gesture::StateClick::Elapsed(void)
{
 SYS_DEBUG_MEMBER(DM_ANDROID_ACCESS);

 try {
    switch (myState) {
        case TOUCH_CLICKED:
            if (click_counter > 0) {
                parent.Clicked(myPosition.x, myPosition.y, click_counter);
            }
            SYS_DEBUG(DL_INFO2, "Clicked: " << click_counter);
        break;
        default:
            SYS_DEBUG(DL_INFO2, "StateClick: ignored (" << myState << ")");
        break;
    }
 } catch (EX::Assert & ex) {
    DEBUG_OUT("Assert failed in Gesture::StateClick::Elapsed(): " << ex.what());
 } catch (std::exception & ex) {
    DEBUG_OUT("Exception caught in Gesture::StateClick::Elapsed(): " << ex.what());
 } catch (...) {
    DEBUG_OUT("Unknown exception in Gesture::StateClick::Elapsed()");
 }
}

void Gesture::StateClick::Step(int event)
{
 SYS_DEBUG_MEMBER(DM_ANDROID_ACCESS);
 SYS_DEBUG(DL_INFO2, "Step(" << event << ")");

 Gesture::TouchMap::const_iterator i = parent.touch_data.begin();

 if (i == parent.touch_data.end()) {
    DEBUG_OUT("ERROR: No data for touch event in the parent!");
    return;
 }

 myPosition = i->second[0];

 int action = event & ACTION_MASK;
 int pointer = (event & ACTION_POINTER_ID_MASK) >> ACTION_POINTER_ID_SHIFT;
 int size = parent.touch_data.size();

 SYS_DEBUG(DL_INFO2, "Action: " << action << ", pointer: " << pointer << ", size: " << size);

 Reset();

 switch (action) {
    case Gesture::ACTION_DOWN:
        // The very first click:
        parent.Clicked(myPosition.x, myPosition.y, 0);
        start_time = i->first;
        ++click_counter;
    break;
    case Gesture::ACTION_UP:
        if (myState != TOUCH_IGNORED)
        {
            int64_t elapsed = i->first - start_time;
            SYS_DEBUG(DL_INFO2, "Elapsed: " << elapsed);
            if (elapsed > CLICK_MIN_TIME && elapsed < CLICK_MAX_TIME) {
                myState = TOUCH_CLICKED;
            } else {
                myState = TOUCH_IGNORED;
            }
        }
    break;
    case Gesture::ACTION_POINTER_DOWN:
        // We have a new pointer (one more finger is just touched)
        parent.CreateEvent_2();
    break;
 }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                                       *
 *       class Gesture::StateMagnify:                                                    *
 *                                                                                       *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

bool Gesture::StateMagnify::Timer(void)
{
 return true; // This class does not work yet
}

void Gesture::StateMagnify::Elapsed(void)
{
}

void Gesture::StateMagnify::Step(int action)
{
 SYS_DEBUG_MEMBER(DM_ANDROID_ACCESS);
 SYS_DEBUG(DL_INFO2, "Step(" << action << ")");
}

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
