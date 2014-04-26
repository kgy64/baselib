/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     Ducktor Navi: The Navigation Project
 * Purpose:     
 * Author:      György Kövesdi <kgy@teledigit.eu>
 * License:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "PathHandler.h"

#include <File/DirHandler.h>

using namespace FILES;

PathHandler::PathHandler(const char * path):
    tags(path, DIR_SEPARATOR_STR"/"),
    filename("")
{
 SYS_DEBUG_FUNCTION(DM_FILE);

 switch (path[0]) {
    case DIR_SEPARATOR: // No break!
    case '.':
        // Nothing to do here
    break;
    default:
        chunks.push_back(".");
    break;
 }

 for (int i = 0; i < tags.size(); ++i) {
    filename = tags[i];
    chunks.push_back(filename);
 }

 SYS_DEBUG(DL_INFO1, "Path is '" << GetDirectoryPath() << "', Filename is '" << GetFilename() << "'");
}

std::string PathHandler::GetDirectoryPath(void) const
{
 std::string result;

 int end = size() - 1;
 for (int i = 0; ; ) {
    result += (*this)[i];
    if (++i >= end) {
        break;
    }
    result += DIR_SEPARATOR_STR;
 }

 return result;
}

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
