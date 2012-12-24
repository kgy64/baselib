#ifndef __TAYLOR_H__
#define __TAYLOR_H__

#include <Filters/FilterBase.h>
#include <boost/scoped_ptr.hpp>

namespace Filter
{
    /// Taylor series
    /*! This class represents N-th order Taylor series. <br>
        Instantiating such filter results in a 0-th order of Taylor polynomial. Call the
        function Taylor<T>::Set(const T &) to set the current level of the polynom. Call that
        function on its return value sets the next (1-st, 2-nd, and so on) order. Any level of
        orders can be handled this way.
        \param T    The data type of the algorythm. Using N-dimensional types here it is possible
                    to handle N-dimensional functions. The basic operators (+,-,*,/,=) must be
                    defined on this data type. */
    template<typename T>
    class Taylor: public I_Filter<T>
    {
     public:
        Taylor(Group & group):
            I_Filter<T>(group),
            constant(0)
        {
        }

     private:
        SYS_DEFINE_CLASS_NAME("Taylor<T>");

        Taylor(Group & group, StateP sp):
            I_Filter<T>(group, sp),
            constant(0)
        {
        }

        Taylor & operator=(const Taylor & other)
        {
            constant = other.constant;
            if (other.filter.get())
                *filter = *other.filter;
            return *this;
        }

        /// Copy another filter
        /*! \warning Only the same type and order of filters can be copied this way. The content
            is not checked, so copying different kind of filters leads to crash. */
        virtual I_Filter<T> & operator=(const I_Filter<T> & other)
        {
            *this = static_cast<const Taylor&>(other);
            return *this;
        }

        /// Sets the coefficient of the current order
        virtual I_Filter<T> & Set(T & data)
        {
            constant = data;
            return *this;
        }

        /*! \retval I_Filter<T>&    Reference of the next order is returned.
            \note   Calling this function creates the next order on demand. */
        virtual I_Filter<T> & Next(unsigned level)
        {
            if (!filter.get())
                filter.reset(new Taylor<T>(FilterBase::myGroup, I_Filter<T>::state));
            if (level > 1)
                return filter->Next(level-1);
            return *filter;
        }

        /// The actual state can be acquired
        virtual operator T() const
        {
            T result(constant);
            if (filter.get() && I_Filter<T>::state->time > 0)
                result += (T)*filter * I_Filter<T>::state->time;
            return result;
        }

        virtual void Reset(void)
        {
            constant = 0;
        }

        /// Next order of the filter
        boost::shared_ptr<I_Filter<T> > filter;

        T constant;
    };
} // namespace Filter

#endif /* __TAYLOR_H__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
