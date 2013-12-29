/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     My Generic C++ Library
 * Purpose:     Memory buffer for temporary usage
 * Author:      György Kövesdi (kgy@teledigit.eu)
 * Licence:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    The interface is similar to the file handler classes.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __SRC_FILE_TEMPORARYBUFFER_H_INCLUDED__
#define __SRC_FILE_TEMPORARYBUFFER_H_INCLUDED__

#include <stdint.h>

#include <Debug/Debug.h>

namespace FILES
{
    class Buffer
    {
     public:
        Buffer(void);
        virtual ~Buffer();

        inline const void * GetData(void) const
        {
            return data;
        }

        inline uint32_t GetSize(void) const
        {
            return position;
        }

        uint32_t Write(void * d, uint32_t size);

        template <typename T>
        inline uint32_t Write(T & d)
        {
            return Write(&d, sizeof d);
        }

     protected:
        uint32_t position;

        uint32_t allocated;

        void * data;

    }; // class FILES::Buffer

} // namespace FILES

#endif /* __SRC_FILE_TEMPORARYBUFFER_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
