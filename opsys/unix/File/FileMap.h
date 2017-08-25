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
            Read_Write,
            Map_Shared      = 0x1000,
            Map_Nonblock    = 0x2000,
            _OPEN_MASK      = 0x0fff
        };

        enum AdviseMode {
            /*! No special treatment.  This is the default. */
            Adv_Normal,

            /*! Expect page references in random order.  (Hence, read ahead may be less useful than normally.) */
            Adv_Random,

            /*! Expect page references in sequential order.  (Hence, pages in the given range can be
             *  aggressively read ahead, and may be freed soon after they are accessed.) */
            Adv_Sequential,

            /*! Expect access in the near future.  (Hence, it might be a good idea to read some pages ahead.) */
            Adv_Willneed,

            /*! Do  not expect access in the near future.  (For the time being, the application is finished with
             *  the given range, so the kernel can free resources associated with it.)  Subsequent accesses of
             *  pages in this range will succeed, but will result either in reloading of the memory contents from
             *  the underlying mapped file (see mmap(2)) or zero-fill-on-demand pages for mappings without an
             *  underlying file. */
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

        inline bool isOk(void) const { return mapped; }

        void Advise(AdviseMode mode);
        void Sync(bool wait = true);
        void Populate(void);

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
