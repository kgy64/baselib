#ifndef __FILEMAP_H__
#define __FILEMAP_H__

#include <string>
#include <sys/types.h>

#include <File/DirHandler.h>
#include <Memory/SPtr.h>
#include <Debug/Debug.h>

namespace FILES
{
    class FileMap
    {
     public:
        enum OpenMode {
            Read_Only,
            Read_Write
        };

     private:
        // Some constructor-like functions, to be compatible with older
        // compilers without c++11 support (instead of delegating these
        // constructors):

        void _FileMap(const char * name, OpenMode mode);

        inline void _FileMap(const std::string & name, OpenMode mode)
        {
            _FileMap(name.c_str(), mode);
        }

     public:
        inline FileMap(const char * name, OpenMode mode = Read_Only)
        {
            _FileMap(name, mode);
        }

        inline FileMap(const std::string & name, OpenMode mode = Read_Only)
        {
            _FileMap(name.c_str(), mode);
        }

        inline FileMap(const DirHandler::iterator & p_it, OpenMode mode = Read_Only)
        {
            _FileMap(p_it.Pathname(), mode);
        }

        virtual ~FileMap();

        void * GetData(void) { return mapped; }

        size_t GetSize(void) { return size; }

     protected:
        int fd;
        void * mapped;
        void * ende;
        size_t size;

     private:
        SYS_DEFINE_CLASS_NAME("FileMap");
    }; // class FileMap

    class FileMap_char: public FileMap
    {
     public:
        FileMap_char(const char * name, FileMap::OpenMode mode = Read_Only):
            FileMap(name, mode)
        {
            actual = GetData();
        }

        virtual ~FileMap_char()
        {
        }

        char * GetData(void) { return (char*) FileMap::GetData(); }

        off_t GetPosition(void) { return (off_t)actual - (off_t)GetData(); }

        int ChrGet(void)
        {
            if (actual >= (char*)ende) {
                ++actual; // Must be incremented here to be able to call UnGet() later
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
    }; // class FileMap_char
} // namespace FILES

#endif /* __FILEMAP_H__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
