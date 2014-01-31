#ifndef __FILEMAP_H__
#define __FILEMAP_H__

#include <string>
#include <sys/types.h>

#include <File/DirHandler.h>
#include <Debug/Debug.h>

namespace FILES
{
    class FileMap
    {
     public:
        enum OpenMode {
            Read_Only,
            Read_Unsafe,
            Read_Write
        };

        enum AdviseMode {
            Adv_Normal,
            Adv_Random,
            Adv_Sequential,
            Adv_Willneed,
            Adv_Dontneed
        };

        FileMap(const char * name, OpenMode mode = Read_Only, size_t p_size = 0);

        inline FileMap(const std::string & name, OpenMode mode = Read_Only):
            FileMap(name.c_str(), mode)
        {
        }

        inline FileMap(const DirHandler::iterator & p_it, OpenMode mode = Read_Only):
            FileMap(p_it.Pathname(), mode)
        {
        }

        FileMap(FileMap & other);

        virtual ~FileMap();

        inline void * GetData(void) { return mapped; }

        inline const void * GetData(void) const { return mapped; }

        inline size_t GetSize(void) const { return size; }

        void Advise(AdviseMode mode);

     protected:
        int fd;
        void * mapped;
        void * ende;
        size_t size;
        OpenMode myMode;

     private:
        SYS_DEFINE_CLASS_NAME("FileMap");

    }; // class FIELS::FileMap

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

#endif /* __FILEMAP_H__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
