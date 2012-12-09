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
        FileHandler(DirPtr & p_dir, const char * p_filename);

        inline FileHandler(DirPtr & p_dir, const std::string & p_filename):
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

        ~FileHandler();

        inline void SetMode(mode_t p_mode)
        {
            mode = p_mode;
        }

        inline DirPtr & GetDirHandler(void)
        {
            return myDir;
        }

        inline std::string GetFullPath(void)
        {
            std::string full_path(myDir->GetPath());
            full_path += DIR_SEPARATOR_STR;
            full_path += myName;
            return full_path;
        }

        void Open(FileMode flag = READ_ONLY);

        void Write(const void * p_data, size_t p_length);

        template <typename T>
        inline void Write(const T & p_data)
        {
            Write((const void *)&p_data, sizeof(T));
        }

        enum {
            DEFAULT_MODE    =   0644
        };

     protected:
        DirPtr myDir;

        std::string myName;

        mode_t mode;

        int fNo;

     private:
        SYS_DEFINE_CLASS_NAME("FILES::FileHandler");

        virtual void BlockedIo(void) {}
    };
}

#endif /* __OPSYS_UNIX_FILE_FILEHANDLER_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
