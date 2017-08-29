/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     My Generic C++ Library
 * Purpose:     Generic interfaces for file I/O
 * Author:      György Kövesdi (kgy@etiner.hu)
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
    DEFINE_EXCEPTION(File_Error, NULL, Error);

    class File_EOF: public File_Error
    {
     public:
        File_EOF(size_t p_bytes_read):
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
        virtual std::string GetFullPath(void) const =0;

    }; // class FILES::Generic

    /// Generic "input" behavior
    class Input: public virtual Generic
    {
     public:
        virtual bool Read(void * p_data, size_t p_length) =0;

    }; // class FILES::Input

    /// Generic "output" behavior
    class Output: public virtual Generic
    {
     public:
        virtual size_t Write(const void * d, size_t size) =0;

        inline Output & operator<<(const      int8_t & data) { Write(&data, sizeof data); return *this; }
        inline Output & operator<<(const     uint8_t & data) { Write(&data, sizeof data); return *this; }
        inline Output & operator<<(const     int16_t & data) { Write(&data, sizeof data); return *this; }
        inline Output & operator<<(const    uint16_t & data) { Write(&data, sizeof data); return *this; }
        inline Output & operator<<(const     int32_t & data) { Write(&data, sizeof data); return *this; }
        inline Output & operator<<(const    uint32_t & data) { Write(&data, sizeof data); return *this; }
        inline Output & operator<<(const     int64_t & data) { Write(&data, sizeof data); return *this; }
        inline Output & operator<<(const    uint64_t & data) { Write(&data, sizeof data); return *this; }
        inline Output & operator<<(const       float & data) { Write(&data, sizeof data); return *this; }
        inline Output & operator<<(const      double & data) { Write(&data, sizeof data); return *this; }
        inline Output & operator<<(const long double & data) { Write(&data, sizeof data); return *this; }

        template <typename T>
        inline Output & operator<<(const T & data)
        {
            data.Write(*this);
            return *this;
        }

    }; // class FILES::Output

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

        void Write(Output & out) const
        {
            out.Write(GetData(), GetSize());
        }

    }; // class FILES::Writeable

} // namespace FILES

#endif /* __SRC_FILE_BASE_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
