/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     
 * Purpose:     
 * Author:      
 * Licence:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __OPSYS_UNIX_FILE_FILEHANDLER_H_INCLUDED__
#define __OPSYS_UNIX_FILE_FILEHANDLER_H_INCLUDED__

#include <string>

#include <File/DirHandler.h>

namespace EX
{
    DEFINE_EXCEPTION(File_Error, "File", Error);
}

namespace FILES
{
    enum FileMode {
        READ_ONLY,
        READ_WRITE,
        APPEND_WRITE
    };

    class FileHandler
    {
     public:
        FileHandler(const DirHandler & p_dir, const char * p_filename);

        inline FileHandler(const DirHandler & p_dir, const std::string & p_filename):
            FileHandler(p_dir, p_filename.c_str())
        {
        }

        FileHandler(const char * p_full_path);
        FileHandler(const char * p_path, const char * p_name);

        inline FileHandler(const std::string & p_full_path):
            FileHandler(p_full_path.c_str())
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

        ~FileHandler();

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

        void Write(const void * p_data, size_t p_length);
        bool Remove(void);

        template <typename T>
        inline void Write(const T & p_data)
        {
            Write((const void *)&p_data, sizeof(T));
        }

        enum {
            DEFAULT_MODE    =   0644
        };

     protected:
        inline FileHandler(void):
            mode(DEFAULT_MODE),
            fNo(-1)
        {
        }

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
}

#endif /* __OPSYS_UNIX_FILE_FILEHANDLER_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
