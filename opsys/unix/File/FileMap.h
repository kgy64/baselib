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

        FileMap(FileMap && other);

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
        FileMap(FileMap & other);

        SYS_DEFINE_CLASS_NAME("FileMap");

    }; // class FIELS::FileMap

} // namespace FILES

#endif /* __FILEMAP_H__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
