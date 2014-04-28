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
#include <Debug/Debug.h>

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

    class Writeable
    {
     public:
        virtual const void * GetData(void) const =0;
        virtual size_t GetSize(void) const =0;

        template <typename T>
        inline const T * GetDataTyped(void) const
        {
            return reinterpret_cast<const T *>(GetData());
        }

    }; // class FILES::Writeable

    class Output: public virtual Generic
    {
     public:
        virtual size_t Write(const void * d, size_t size) =0;

        Output & WriteChecked(const void * data, size_t size)
        {
            size_t written = Write(data, size);
            if (written != size) {
                throw EX::File_Error() << "Written " << written << " bytes instead of " << size;
            }
            return *this;
        }

        inline Output & operator<<(const Writeable & data)
        {
            return WriteChecked(data.GetData(), data.GetSize());
        }

        Output & operator<<(const   int8_t & data) { return WriteChecked(&data, sizeof data); }
        Output & operator<<(const  uint8_t & data) { return WriteChecked(&data, sizeof data); }
        Output & operator<<(const  int16_t & data) { return WriteChecked(&data, sizeof data); }
        Output & operator<<(const uint16_t & data) { return WriteChecked(&data, sizeof data); }
        Output & operator<<(const  int32_t & data) { return WriteChecked(&data, sizeof data); }
        Output & operator<<(const uint32_t & data) { return WriteChecked(&data, sizeof data); }
        Output & operator<<(const  int64_t & data) { return WriteChecked(&data, sizeof data); }
        Output & operator<<(const uint64_t & data) { return WriteChecked(&data, sizeof data); }

    }; // class FILES::Output

} // namespace FILES

#endif /* __SRC_FILE_BASE_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
