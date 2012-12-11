/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     
 * Purpose:     
 * Author:      
 * Licence:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __OPSYS_UNIX_FILE_DIRHANDLER_H_INCLUDED__
#define __OPSYS_UNIX_FILE_DIRHANDLER_H_INCLUDED__

#include <string>
#include <Exceptions/ICExceptions.h>
#include <Memory/SPtr.h>

#include <Debug/Debug.h>

#define     DIR_SEPARATOR       '/'
#define     DIR_SEPARATOR_STR   "/"

namespace EX
{
    DEFINE_EXCEPTION(DIR_Exception, "Dir", Error);
}

namespace FILES
{
    class DirHandler
    {
     public:
        DirHandler(const char * p_path);

        inline DirHandler(const std::string & p_path):
            DirHandler(p_path.c_str())
        {
        }

        ~DirHandler();

        inline void SetMode(mode_t p_mode)
        {
            mode = p_mode;
        }

        DirHandler SubDir(const char * p_path) const;

        inline DirHandler SubDir(const std::string & p_path) const
        {
            return SubDir(p_path.c_str());
        }

        bool Create(bool recursive = false);
        bool Create(const char * sub_path, bool recursive = false);

        inline void Create(const std::string & sub_path, bool recursive = false)
        {
            Create(sub_path.c_str(), recursive);
        }

        const std::string & GetPath(void) const
        {
            return path;
        }

        static bool IsExist(const char * path);

        static inline bool IsExist(const std::string & path)
        {
            return IsExist(path.c_str());
        }

        static bool IsDirectory(const char * path);

        static inline bool IsDirectory(const std::string & path)
        {
            return IsDirectory(path.c_str());
        }

        enum {
            DEFAULT_MODE    =   0775
        };

     private:
        SYS_DEFINE_CLASS_NAME("FILES::DirHandler");

        std::string path;

        mode_t mode;
    };

    typedef SPtr<DirHandler> DirPtr;
}

#endif /* __OPSYS_UNIX_FILE_DIRHANDLER_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
