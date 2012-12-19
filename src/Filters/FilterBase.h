#ifndef __FILTERBASE_H__
#define __FILTERBASE_H__

#include <glibmm/refptr.h>
#include <Base/Linked.h>
#include <Debug/Debug.h>

SYS_DECLARE_MODULE(DM_FILTER);

namespace Filter
{
    class Group;
    typedef float filter_time_t;

    class FilterBase
    {
     friend class Linked<FilterBase>;

     public:
        virtual void Reset(void) =0;
        virtual void At(filter_time_t time) =0;

        virtual void Print(std::ostream & os) const =0;

     protected:
        FilterBase(Group & group);
        virtual ~FilterBase();

        Group & myGroup;

     private:
        SYS_DEFINE_CLASS_NAME("FilterBase");

        FilterBase * next;
    };

    class Group: public Linked<FilterBase>
    {
     public:
        /// Statically initialized version
        Group(void)
        {
        }

        /// Initialized by the constructor
        Group(int)
        {
            Linked<FilterBase>::Reset();
        }

        void Reset(void);
    };

    struct _FilterState
    {
        _FilterState(void):
            time(0),
            references(1)
        {
        }

        void reference(void)
        {
            ++references;
        }

        void unreference(void)
        {
            if (!--references)
                delete this;
        }

        filter_time_t time;

        int references;

        void Print(std::ostream & os) const
        {
            os << "at " << time;
        }
    };

    typedef Glib::RefPtr<_FilterState> StateP;

    template <typename T>
    class I_Filter: public FilterBase
    {
     protected:
        I_Filter(Group & group):
            FilterBase(group),
            state(new _FilterState),
            time_offset(0)
        {
        }

        I_Filter(Group & group, StateP & sp):
            FilterBase(group),
            state(sp)
        {
        }

        StateP state;

        filter_time_t time_offset;

     public:
        void Step(filter_time_t elapsed)
        {
            SYS_DEBUG_MEMBER(DM_FILTER);
            state->time += elapsed;
            SYS_DEBUG(DL_INFO1, "Step(" << elapsed << ") to " << state->time);
        }

        virtual void At(filter_time_t time)
        {
            SYS_DEBUG_MEMBER(DM_FILTER);
            state->time = time;
            SYS_DEBUG(DL_INFO1, "At(" << state->time << ")");
        }

        /// The result (output of the filter)
        /*! \param elapsed The step of time relative to the current state (previous read) of the filter. */
        T OutStep(filter_time_t elapsed)
        {
            Step(elapsed);
            return *this;
        }

        T OutAt(filter_time_t time_position)
        {
            At(time_position);
            return *this;
        }

        virtual operator T() const
        {
            return T();
        }

        virtual void In(const T & data, filter_time_t elapsed)
        {
        }

        virtual void Print(std::ostream & os) const
        {
            os << time_offset;
        }

        virtual I_Filter & operator=(const I_Filter & other) =0;
        virtual I_Filter<T> & Set(T & data) =0;
        virtual I_Filter<T> & Next(unsigned level = 1) =0;

     private:
        SYS_DEFINE_CLASS_NAME("I_Filter");
    };
} // namespace Filter

inline std::ostream & operator<<(std::ostream & os, const Filter::FilterBase & f)
{
 f.Print(os);
 return os;
}

#endif /* __FILTERBASE_H__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
