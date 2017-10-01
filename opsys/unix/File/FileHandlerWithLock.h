/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     My Generic C++ Library
 * Purpose:     
 * Author:      György Kövesdi (kgy@etiner.hu)
 * Licence:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __BASELIB_OPSYS_UNIX_FILE_FILEHANDLERWITHLOCK_H_INCLUDED__
#define __BASELIB_OPSYS_UNIX_FILE_FILEHANDLERWITHLOCK_H_INCLUDED__

#include <File/FileHandler.h>
#include <Threads/Mutex.h>

namespace FILES
{
    class FileHandlerWithLock: public FILES::FileHandler
    {
     public:
        inline FileHandlerWithLock(void):
            FileHandler()
        {
        }

        inline FileHandlerWithLock(const char * p_full_path):
            FileHandler(p_full_path)
        {
        }

        inline FileHandlerWithLock(const std::string & p_full_path):
            FileHandler(p_full_path)
        {
        }

        inline FileHandlerWithLock(const DirHandler & p_dir, const char * p_filename):
            FileHandler(p_dir, p_filename)
        {
        }

        inline FileHandlerWithLock(const DirHandler & p_dir, const std::string & p_filename):
            FileHandler(p_dir, p_filename)
        {
        }

        inline FileHandlerWithLock(const char * p_path, const char * p_name):
            FileHandler(p_path, p_name)
        {
        }

        inline FileHandlerWithLock(const std::string & p_path, const std::string & p_name):
            FileHandler(p_path, p_name)
        {
        }

        inline FileHandlerWithLock(const DirHandler::iterator & p_it):
            FileHandler(p_it)
        {
        }

        virtual Threads::Mutex & GetMutex(void)
        {
            return myMutex;
        }

     private:
        SYS_DEFINE_CLASS_NAME("FILES::FileHandlerWithLock");

        Threads::Mutex myMutex;

    }; // class FileHandlerWithLock

} // namespace FILES

#endif /* __BASELIB_OPSYS_UNIX_FILE_FILEHANDLERWITHLOCK_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
