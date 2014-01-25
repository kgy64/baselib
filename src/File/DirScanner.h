/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     Ducktor Navi: The Navigation Project
 * Purpose:     class DirScanner: scans a given path recursively
 * Author:      György Kövesdi <kgy@teledigit.eu>
 * License:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __BASELIB_SRC_FILE_DIRSCANNER_H_INCLUDED__
#define __BASELIB_SRC_FILE_DIRSCANNER_H_INCLUDED__

#include <string>
#include <map>
#include <boost/shared_ptr.hpp>

namespace FILES
{
    class DirScanner;

    typedef boost::shared_ptr<DirScanner> DirScanPtr;

    /// Recursive directory scanner
    class DirScanner
    {
        protected:
            /*! \param  path    The full pathname of the directory to be scanned. */
            inline DirScanner(const std::string & path):
                path(path)
            {
            }

            /// What to do with a file or directory
            enum EntryType
            {
                /// Ignore: forget about it
                /*! If the function \ref CheckName() returns this value, then no other operations will
                 *  be done with the given entry. */
                T_IGNORE = 0,

                /// Do recursive scan immediately
                /*! If the function \ref CheckName() returns this value, then the function \ref CreateSubdir()
                 *  will be called immediately, executing a recursive scan.
                 *  \note   Because it means recursion, this value must be returned only on directories.
                 *  \note   This performs the execution in storage order (not alphabetical order). */
                T_SCAN_NOW,

                /*! If the function \ref CheckName() returns this value, then the entry name will be stored
                 *  alphabetically, and the recursion will be executed after all other \ref T_SCAN_NOW
                 *  executions.
                 *  \note   Because it means recursion, this value must be returned only on directories.
                 *  \note   This performs the execution in alphabetical order. */
                T_SCAN_SORT,

                /*! If the function \ref CheckName() returns this value, then the function \ref GotEntry() will
                 *  be called immediately.
                 *  \note   The recusrion is stopped here, even if the path is a file or directory.
                 *  \note   This performs the execution in storage order (not alphabetical order). */
                T_GOT_NOW,

                /*! If the function \ref CheckName() returns this value, then the entry name will be stored
                 *  alphabetically, and the function \ref CheckName() will be called after all other \ref T_GOT_NOW
                 *  executions.
                 *  \note   The recusrion is stopped here, even if the path is a file or directory.
                 *  \note   This performs the execution in alphabetical order. */
                T_GOT_SORT
            };

            /// Do recursion: create another instance on the given path
            /*! This function can be called on directory entries.
             *  \param  pathname    The full pathname of the directory. */
            virtual DirScanPtr CreateSubdir(const std::string & pathname) =0;

            /// End of recursion
            /*! This function can be called on file or directory entries, but no further recursion will be done.
             *  \param  pathname    The full pathname of the entry (file or directory).
             *  \param  name        The name without path. */
            virtual void GotEntry(const std::string & pathname, const std::string & name) =0;

            /// This function decides what to do with the given entry
            /*! \param  name        The name of the entry (without path). Can be a directory or a file.
             *  \retval EntryType   The operation to do with the entry. See \ref EntryType for details. */
            virtual EntryType CheckName(const std::string & name) const =0;

            /// The directory scan is finished
            /*! This function will be called after all entries in the current directory has been processed.<br>
             *  Note that it will be called only if there was no exception during processing. */
            virtual void finished(void)
            {
            }

            typedef std::map<std::string, std::string> name_map_t;

            /// The current path this class works on
            std::string path;

        public:
            /// Start the directory scan
            /*! This function will call the member function \ref CheckName() for each directory entry to decide
             *  what to do.
             *  \note   The directory scans (see \ref CreateSubdir()) will be executed before entry operations
             *          (see \ref GotEntry()) except the immediate executions (see \ref EntryType).*/
            inline void Scan(void)
            {
                workOnDir();
                finished();
            }

        private:
            void workOnDir(void);

    }; // class DB::DatabaseIndexer::DirScanner

} // namespace FILES

#endif /* __BASELIB_SRC_FILE_DIRSCANNER_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
