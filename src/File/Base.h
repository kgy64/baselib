/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     My Generic C++ Library
 * Purpose:     Generic interfaces for file I/O
 * Author:      György Kövesdi (kgy@teledigit.eu)
 * Licence:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __SRC_FILE_BASE_H_INCLUDED__
#define __SRC_FILE_BASE_H_INCLUDED__

#include <stddef.h>

#include <Exceptions/Exceptions.h>

namespace EX
{
    DEFINE_EXCEPTION(File_Error, "File", Error);

    class File_EOF: public File_Error
    {
     public:
        File_EOF(size_t p_bytes_read):
            File_Error("EOF Exception"),
            myBytes(p_bytes_read)
        {
        }

        inline size_t GetBytes(void)
        {
            return myBytes;
        }

        template <typename T>
        inline File_EOF & operator<<(const T & value)
        {
            static_cast<File_Error &>(*this) << value;
            return *this;
        }

     protected:
        size_t myBytes;
    };
}

namespace FILES
{
    class Generic
    {
     public:
        virtual off_t Tell(void) const =0;

    }; // class FILES::Generic

    class Input: public virtual Generic
    {
     public:
        virtual bool Read(void * p_data, size_t p_length) =0;

    }; // class FILES::Input

    class Output: public virtual Generic
    {
     public:
        virtual size_t Write(const void * d, size_t size) =0;

        template <typename T>
        Output & operator<<(const T & data)
        {
            size_t written = Write(&data, sizeof data);
            if (written != sizeof data) {
                throw EX::File_Error() << "Written " << written << " bytes instead of " << sizeof data;
            }
            return *this;
        }

    }; // class FILES::Output

    class Writeable
    {
     public:
        virtual const void * GetData(void) const =0;
        virtual size_t GetSize(void) const =0;

    }; // class FILES::Writeable

} // namespace FILES

#endif /* __SRC_FILE_BASE_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
