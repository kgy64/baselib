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

#include <File/Base.h>
#include <Debug/Debug.h>

namespace FILES
{
    class Buffer: public FILES::Writeable, public FILES::Output
    {
     public:
        Buffer(void);
        virtual ~Buffer();

        virtual size_t Write(const void * d, size_t size) override;
        virtual const void * GetData(void) const override;
        virtual size_t GetSize(void) const override;
        virtual off_t Tell(void) const override;

     protected:
        size_t position;

        size_t allocated;

        void * data;

    }; // class FILES::Buffer

} // namespace FILES

inline FILES::Output & operator<<(FILES::Output & out, const FILES::Buffer & data)
{
 return out << static_cast<const FILES::Writeable &>(data);
}

#endif /* __SRC_FILE_TEMPORARYBUFFER_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
