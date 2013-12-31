/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     My Generic C++ Library
 * Purpose:     Generic file handler classes for read and write
 * Author:      György Kövesdi (kgy@teledigit.eu)
 * Licence:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __OPSYS_UNIX_FILE_FILEHANDLER_H_INCLUDED__
#define __OPSYS_UNIX_FILE_FILEHANDLER_H_INCLUDED__

#include <string>

#include <File/Base.h>
#include <File/FileFunctions.h>
#include <File/DirHandler.h>

namespace Threads
{
    class Mutex;
}

namespace FILES
{
    enum FileMode {
        READ_ONLY,
        READ_WRITE,
        APPEND_WRITE
    };

    class FileHandler: public FILES::Output, public FILES::Input
    {
     public:
        FileHandler(const DirHandler & p_dir, const char * p_filename);
        FileHandler(const char * p_full_path);
        FileHandler(const char * p_path, const char * p_name);

        inline FileHandler(void):
            mode(DEFAULT_MODE),
            fNo(-1)
        {
        }

        inline FileHandler(const std::string & p_full_path):
            FileHandler(p_full_path.c_str())
        {
        }

        inline FileHandler(const DirHandler & p_dir, const std::string & p_filename):
            FileHandler(p_dir, p_filename.c_str())
        {
        }

        inline FileHandler(const std::string & p_path, const std::string & p_name):
            FileHandler(p_path.c_str(), p_name.c_str())
        {
        }

        inline FileHandler(const DirHandler::iterator & p_it):
            FileHandler(p_it.Pathname())
        {
        }

        VIRTUAL_IF_DEBUG ~FileHandler();

        inline void SetMode(mode_t p_mode)
        {
            mode = p_mode;
        }

        inline const std::string & GetDir(void) const
        {
            return myDir;
        }

        inline DirPtr GetDirHandler(void) const
        {
            return DirPtr(new DirHandler(GetDir()));
        }

        inline std::string GetFullPath(void) const
        {
            std::string full_path(myDir);
            full_path += DIR_SEPARATOR_STR;
            full_path += myName;
            return full_path;
        }

        void Open(FileMode flag = READ_ONLY);
        void OpenSpecial(FileMode flag);

        virtual bool Read(void * p_data, size_t p_length) override;
        virtual size_t Write(const void * p_data, size_t p_length) override;

        virtual off_t Tell(void) const override
        {
            off_t result = lseek(fNo, 0, SEEK_CUR);
            ASSERT(result != (off_t)-1, "Cannot seek");
            return result;
        }

        inline off_t Seek(off_t p_seek, int p_whence = SEEK_SET) const
        {
            off_t result = lseek(fNo, p_seek, p_whence);
            ASSERT(result != (off_t)-1, "Cannot seek");
            return result;
        }

        inline bool Remove(void)
        {
            return FILES::Remove(GetFullPath().c_str());
        }

        inline size_t Write(const Writeable & buf)
        {
            return Write(buf.GetData(), buf.GetSize());
        }

        template <typename T>
        inline bool Read(T & p_data)
        {
            return Read((void *)&p_data, sizeof(T));
        }

        enum {
            DEFAULT_MODE    =   0644
        };

        virtual Threads::Mutex & GetMutex(void)
        {
            throw EX::File_Error() << "FileHandler::GetLock() is called on a non-lockable handler";
        }

     protected:
        std::string myDir;

        std::string myName;

        mode_t mode;

        int fNo;

     private:
        SYS_DEFINE_CLASS_NAME("FILES::FileHandler");

        virtual void BlockedIo(void) {}
    };

    class StdInput: public FileHandler
    {
        inline StdInput(void):
            FileHandler()
        {
            fNo = 0;
        }
    };

    class StdOutput: public FileHandler
    {
        inline StdOutput(void):
            FileHandler()
        {
            fNo = 1;
        }
    };

    class StdError: public FileHandler
    {
        inline StdError(void):
            FileHandler()
        {
            fNo = 2;
        }
    };

} // namespace FILES

#endif /* __OPSYS_UNIX_FILE_FILEHANDLER_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
