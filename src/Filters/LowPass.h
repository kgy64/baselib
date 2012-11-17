#ifndef __LOWPASS_H__
#define __LOWPASS_H__

#include <Filters/FilterBase.h>

namespace Filter
{
    template<typename T>
    class LowPass
    {
     public:
        LowPass(filter_time_t p_time_constant):
            state(0),
            time_constant(p_time_constant)
        {
        }

        virtual ~LowPass()
        {
        }

        void In(T input, filter_time_t elapsed)
        {
            T step_size = elapsed / time_constant;
            if (step_size <= 0)
                return;
            if (step_size > 1)
                step_size = 1;
            state = step_size * input + (1-step_size) * state;
        }

        operator T() const
        {
            return state;
        }

     protected:
        T state;

        filter_time_t time_constant;

     private:
        SYS_DEFINE_CLASS_NAME("Filter::LowPass<T>");
    };
}

#endif /* __LOWPASS_H__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
