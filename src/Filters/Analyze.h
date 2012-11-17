#ifndef __ANALYZE_H__
#define __ANALYZE_H__

#include <Filters/FilterBase.h>
#include <memory>
#include <iostream>

namespace Filter
{
    template<typename T>
    class Analyze: public I_Filter<T>
    {
     public:
        Analyze(Group & group):
            I_Filter<T>(group),
            order(0.0),
            valid(false)
        {
        }

     private:
        SYS_DEFINE_CLASS_NAME("Analyze<T>");

     protected:
        Analyze(Group & group, StateP sp, filter_time_t p_ord):
            I_Filter<T>(group, sp),
            order(p_ord),
            valid(false)
        {
        }

        Analyze & operator=(const Analyze & other)
        {
            state = other.state;
            valid = other.valid;
            if (other.next.get())
                *next = *other.next;
            return *this;
        }

        virtual I_Filter<T> & operator=(const I_Filter<T> & other)
        {
            *this = static_cast<const Analyze&>(other);
            return *this;
        }

        virtual void In(const T & data, filter_time_t time)
        {
            SYS_DEBUG_MEMBER(DM_FILTER);
            SYS_DEBUG(DL_GPS, "order=" << order << ", data: " << data << ", time=" << time);
            if (valid && next.get()) {
                filter_time_t current_offset = time / 2.0;

                filter_time_t next_level_step = next->I_Filter<T>::time_offset + current_offset;
                next->In((data-state)/time, next_level_step);

                next->I_Filter<T>::time_offset = current_offset;
            }
            state = data;
            valid = true;
            I_Filter<T>::state->time = 0;
        }

        T GetMyState(filter_time_t add_time = 0) const
        {
            SYS_DEBUG_MEMBER(DM_FILTER);
            if (!valid)
                return T(0);
            filter_time_t my_offset = I_Filter<T>::time_offset + add_time;
            SYS_DEBUG(DL_GPS, "order=" << order << ", state=" << state << ", offset=" << my_offset << ", time=" <<
                    I_Filter<T>::state->time << ", full-time=" << my_offset + I_Filter<T>::state->time);
            T result(state);
            if (next.get())
                result += next->GetMyState(my_offset) * (my_offset + I_Filter<T>::state->time);
            SYS_DEBUG(DL_GPS, "result=" << result);
            return result;
        }

        virtual operator T() const
        {
            SYS_DEBUG_MEMBER(DM_FILTER);
            return GetMyState();
        }

        virtual I_Filter<T> & Set(T & data)
        {
            SYS_DEBUG_MEMBER(DM_FILTER);
            SYS_DEBUG(DL_GPS, "order=" << order << ", state: " << state << ", time=" << I_Filter<T>::state->time);
            if (valid)
                data = state;
            return *this;
        }

        virtual I_Filter<T> & Next(unsigned level)
        {
            if (!next.get())
                next.reset(new Analyze<T>(FilterBase::myGroup, I_Filter<T>::state, order+1.0));
            if (level > 1)
                return next->Next(level-1);
            return *next;
        }

        virtual void Reset(void)
        {
            valid = false;
        }

        void ResetRecursive(void)
        {
            Reset();
            if (next.get())
                next->ResetRecursive();
        }

        virtual void Print(std::ostream & os) const
        {
            I_Filter<T>::state->Print(os);
            os << ": ";
            PrintMyself(os);
        }

        void PrintMyself(std::ostream & os) const
        {
            os << "[" << order << "] ";
            if (valid) {
                os << state << " ";
                I_Filter<T>::Print(os);
            } else {
                os << "invalid";
            }
            if (next.get()) {
                os << " ";
                next->PrintMyself(os);
            }
        }

        std::auto_ptr<Analyze<T> > next;

        T state;

        filter_time_t order;

        bool valid;
    };
}

#endif /* __ANALYZE_H__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
