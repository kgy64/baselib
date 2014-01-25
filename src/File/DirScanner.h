/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     Ducktor Navi: The Navigation Project
 * Purpose:     
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

    class DirScanner
    {
        public:
            inline void Scan(void)
            {
                workOnDir();
                finished();
            }

        protected:
            enum EntryType
            {
                T_IGNORE = 0,
                T_SCAN_NOW,
                T_SCAN_SORT,
                T_GOT_NOW,
                T_GOT_SORT
            };

            virtual DirScanPtr CreateSubdir(const std::string & path) =0;
            virtual void GotEntry(const std::string & path, const std::string & name) =0;
            virtual EntryType CheckName(const std::string & name) const =0;

            virtual void finished(void)
            {
            }

            inline DirScanner(const std::string & path):
                path(path)
            {
            }

            typedef std::map<std::string, std::string> name_map_t;

            void workOnDir(void);

            std::string path;

    }; // class DB::DatabaseIndexer::DirScanner

} // namespace FILES

#endif /* __BASELIB_SRC_FILE_DIRSCANNER_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
