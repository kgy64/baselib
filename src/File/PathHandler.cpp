/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     Baselib - Basic System Access Library
 * Purpose:     
 * Author:      György Kövesdi <kgy@etiner.hu>
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
    case DIR_SEPARATOR:
        // Note: the tokenizer eats the leading separators, so add a slash at the beginning:
        chunks.push_back(DIR_SEPARATOR_STR);
    break;
    case '.':
        // Nothing to do here
    break;
    default:
        // Relative to the current directory:
        chunks.push_back(".");
    break;
 }

 for (int i = 0; i < tags.size(); ++i) {
    filename = tags[i];
    chunks.push_back(filename);
 }

 // Note: the tokenizer eats the trailing separators, so check the slash at the end
 //       to detect directory path:
 if (*path && path[strlen(path)-1] == DIR_SEPARATOR) {
    filename = "";
    chunks.push_back(filename);
 }

 SYS_DEBUG(DL_INFO1, "Path is '" << GetDirectoryPath() << "', Filename is '" << GetFilename() << "'");
}

std::string PathHandler::GetDirectoryPath(void) const
{
 SYS_DEBUG_FUNCTION(DM_FILE);

 std::string result;

 int end = size() - 1;
 for (int i = 0; i < end; ++i) {
    const char * current = (*this)[i];
    SYS_DEBUG(DL_INFO1, "Appending '" << current << "'");
    result += current;
    // Add a separator if it is not a single slash:
    if ((current[0] != DIR_SEPARATOR) || current[1]) {
        result += DIR_SEPARATOR_STR;
    }
 }

 return result;
}

std::string PathHandler::GetFullPath(void) const
{
 SYS_DEBUG_FUNCTION(DM_FILE);

 std::string result = GetDirectoryPath();

 if (*filename) {
    result += DIR_SEPARATOR_STR;
    result += filename;
 }

 return result;
}

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
