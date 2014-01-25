/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     Ducktor Navi: The Navigation Project
 * Purpose:     
 * Author:      György Kövesdi <kgy@teledigit.eu>
 * License:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "DirScanner.h"

#include <File/DirHandler.h>
#include <Debug/Debug.h>

using namespace FILES;

void DirScanner::workOnDir(void)
{
 SYS_DEBUG_MEMBER(DM_DB_IDX);

 name_map_t names;
 name_map_t subdirs;

 for (FILES::DirHandler::iterator i(path); i; ++i) {
    std::string entry_name = i.Name();
    switch (CheckName(entry_name)) {
        case T_SCAN_NOW:
            CreateSubdir(i.Pathname())->Scan();
        break;
        case T_SCAN_SORT:
            subdirs[entry_name] = i.Pathname();
        break;
        case T_GOT_NOW:
            GotEntry(i.Pathname(), entry_name);
        break;
        case T_GOT_SORT:
            names[entry_name] = i.Pathname();
        break;
        default:
            continue;
        break;
    }
 }

 SYS_DEBUG(DL_INFO2, "Number of directories under '" << path << "': " << subdirs.size());
 for (name_map_t::const_iterator i = subdirs.begin(); i != subdirs.end(); ++i) {
    CreateSubdir(i->second)->Scan();
 }

 SYS_DEBUG(DL_INFO2, "Number of files under '" << path << "': " << names.size());
 for (name_map_t::const_iterator i = names.begin(); i != names.end(); ++i) {
    GotEntry(i->second, i->first);
 }
}

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
