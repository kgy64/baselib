/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     Generic Library
 * Purpose:     File mappers for specific types
 * Author:      György Kövesdi <kgy@etiner.hu>
 * License:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __BASELIB_SRC_FILE_FILEMAPTYPED_H_INCLUDED__
#define __BASELIB_SRC_FILE_FILEMAPTYPED_H_INCLUDED__

#include <File/FileMap.h>

namespace FILES
{
    template <typename T>
    class FileMap_typed: public FileMap
    {
     public:
        inline FileMap_typed(const char * name, FileMap::OpenMode mode = Read_Only, size_t p_size = 0):
            FileMap(name, mode, p_size * sizeof(T))
        {
        }

        virtual inline ~FileMap_typed()
        {
        }

        T & operator*()
        {
            return *reinterpret_cast<T *>(FileMap::GetData());
        }

        const T & operator*() const
        {
            return *reinterpret_cast<const T *>(FileMap::GetData());
        }

        T & operator[](off_t index)
        {
            return reinterpret_cast<T *>(FileMap::GetData())[index];
        }

        const T & operator[](off_t index) const
        {
            return reinterpret_cast<const T *>(FileMap::GetData())[index];
        }

     private:
        SYS_DEFINE_CLASS_NAME("FileMap_typed");

    }; // class FIELS::FileMap_typed

    class FileMap_char: public FileMap_typed<char>
    {
     public:
        inline FileMap_char(const char * name, FileMap::OpenMode mode = Read_Only, size_t p_size = 0):
            FileMap_typed(name, mode, p_size)
        {
            actual = &**this;
        }

        virtual inline ~FileMap_char()
        {
        }

        off_t GetPosition(void) { return (off_t)actual - (off_t)&*this; }

        int ChrGet(void)
        {
            if (actual >= (char*)ende) {
                if (actual == (char*)ende) {
                    ++actual; // Must be incremented here to be able to call UnGet() later
                }
                return -1;
            }
            return *actual++;
        }

        void UnGet(void)
        {
            if (actual > (char*)mapped)
                --actual;
        }

     protected:
        char * actual;

     private:
        SYS_DEFINE_CLASS_NAME("FileMap_char");

    }; // class FILES::FileMap_char

} // namespace FILES

#endif /* __BASELIB_SRC_FILE_FILEMAPTYPED_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
