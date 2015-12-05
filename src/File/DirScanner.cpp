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

using namespace FILES;

/// Executes the recursive scan
void DirScanner::workOnDir(void)
{
 SYS_DEBUG_MEMBER(DM_FILE);

 name_map_t names;
 name_map_t subdirs;

 for (FILES::DirHandler::iterator i(path); i; ++i) {
    std::string entry_name = i.Name();
    switch (CheckName(entry_name)) {
        case T_SCAN_NOW:
            SYS_DEBUG(DL_INFO2, "Subdirectory '" << entry_name << "' is scanned now");
            CreateSubdir(i.Pathname(), entry_name)->Scan();
        break;
        case T_SCAN_SORT:
            SYS_DEBUG(DL_INFO2, "Subdirectory '" << entry_name << "' is to be scanned");
            subdirs[entry_name] = i.Pathname();
        break;
        case T_GOT_NOW:
            SYS_DEBUG(DL_INFO2, "File '" << entry_name << "' is processed now");
            GotEntry(i.Pathname(), entry_name);
        break;
        case T_GOT_SORT:
            SYS_DEBUG(DL_INFO2, "File '" << entry_name << "' is to be processed");
            names[entry_name] = i.Pathname();
        break;
        case T_IGNORE:
            SYS_DEBUG(DL_INFO2, "Name '" << entry_name << "' is ignored");
            continue;
        break;
        default:
            DEBUG_OUT("Wrong return value from CheckName() at path '" << i.Pathname() << "'");
            continue;
        break;
    }
 }

 SYS_DEBUG(DL_INFO2, "Number of directories under '" << path << "': " << subdirs.size());
 for (name_map_t::const_iterator i = subdirs.begin(); i != subdirs.end(); ++i) {
    CreateSubdir(i->second, i->first)->Scan();
 }

 SYS_DEBUG(DL_INFO2, "Number of files under '" << path << "': " << names.size());
 for (name_map_t::const_iterator i = names.begin(); i != names.end(); ++i) {
    GotEntry(i->second, i->first);
 }
}

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
