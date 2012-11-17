#ifndef __EXTRAPOLATE_H__
#define __EXTRAPOLATE_H__

#include <Filters/FilterBase.h>

namespace Filter
{
    template<typename T>
    class Extrapolate: public I_Filter<T>
    {
     public:
        Extrapolate(Group & group):
            I_Filter<T>(group)
        {
        }

     private:
    };
} // namespace Filter

#endif /* __EXTRAPOLATE_H__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
