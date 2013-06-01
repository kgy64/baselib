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
#include <sys/types.h>
#include <dirent.h>

#include <Exceptions/Exceptions.h>
#include <File/FilePtrs.h>

#include <Debug/Debug.h>

#define     DIR_SEPARATOR       '/'
#define     DIR_SEPARATOR_STR   "/"

SYS_DECLARE_MODULE(DM_FILE);

namespace EX
{
    DEFINE_EXCEPTION(DIR_Exception, "Dir", Error);
}

namespace FILES
{
    class DirHandler
    {
        // Some constructor-like functions, to be compatible with older
        // compilers without c++11 support (instead of delegating these
        // constructors):

        void _DirHandler(const char * p_path);

        inline void _DirHandler(const std::string & p_path)
        {
            _DirHandler(p_path.c_str());
        }

     public:
        class iterator;

        inline DirHandler(const char * p_path)
        {
            _DirHandler(p_path);
        }

        inline DirHandler(const std::string & p_path)
        {
            _DirHandler(p_path);
        }

        inline DirHandler(const DirHandler::iterator & p_iter)
        {
            _DirHandler(p_iter.Pathname());
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
        static bool IsFile(const char * path, bool p_any = true);

        static inline bool IsDirectory(const std::string & path)
        {
            return IsDirectory(path.c_str());
        }

        static inline bool IsFile(const std::string & path, bool p_any = true)
        {
            return IsFile(path.c_str(), p_any);
        }

        static inline bool IsDirectory(const DirHandler::iterator & p_it)
        {
            return IsDirectory(p_it.Pathname());
        }

        static inline bool IsFile(const DirHandler::iterator & p_it, bool p_any = true)
        {
            return IsFile(p_it.Pathname(), p_any);
        }

        inline bool IsDirectory(void) const
        {
            return IsDirectory(GetPath());
        }

        enum {
            DEFAULT_MODE    =   0775
        };

        class iterator
        {
            // Some constructor-like functions, to be compatible with older
            // compilers without c++11 support (instead of delegating these
            // constructors):

            void _iterator(const char * p_parent);

            inline void _iterator(const std::string & p_parent)
            {
                _iterator(p_parent.c_str());
            }

         friend class DirHandler;

         public:
            iterator(const char * p_parent);

            inline iterator(const std::string & p_parent)
            {
                _iterator(p_parent.c_str());
            }

            ~iterator();

            inline bool operator==(const iterator & p_other) const
            {
                // Probably it means both are NULL:
                return actualEntry == p_other.actualEntry;
            }

            inline operator bool() const
            {
                return actualEntry;
            }

            inline bool IsDirectory(void) const
            {
                return DirHandler::IsDirectory(Pathname());
            }

            inline bool IsFile(bool p_any = true) const
            {
                return DirHandler::IsFile(Pathname(), p_any);
            }

            std::string Name(void) const;
            std::string Pathname(void) const;

            iterator & operator++();

            iterator operator*() const;

            template <class C>
            boost::shared_ptr<C> GetHandler(void) const
            {
                SYS_DEBUG_MEMBER(DM_FILE);
                return boost::shared_ptr<C>(new C(*this));
            }

         private:
            SYS_DEFINE_CLASS_NAME("File::DirHandler::iterator");

            inline iterator(const DirHandler & p_parent)
            {
                _iterator(p_parent.GetPath());
            }

            inline iterator(void):
                myDir(NULL),
                actualEntry(NULL)
            {
            }

            std::string myPath;

            DIR * myDir;

            dirent myEntry;

            dirent * actualEntry;
        };

        inline iterator begin() const
        {
            return iterator(*this);
        }

        static inline iterator end()
        {
            return iterator();
        }

     private:
        SYS_DEFINE_CLASS_NAME("FILES::DirHandler");

        std::string path;

        mode_t mode;
    };
}

#endif /* __OPSYS_UNIX_FILE_DIRHANDLER_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
