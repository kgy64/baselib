/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     
 * Purpose:     
 * Author:      
 * Licence:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include "DirHandler.h"

using namespace FILES;

DirHandler::DirHandler(const char * p_path):
    path(p_path),
    mode(DEFAULT_MODE)
{
 SYS_DEBUG_MEMBER(DM_FILE);
}

DirHandler::~DirHandler()
{
 SYS_DEBUG_MEMBER(DM_FILE);
}

DirHandler DirHandler::SubDir(const char * p_path) const
{
 SYS_DEBUG_MEMBER(DM_FILE);
 std::string full_path(path);
 full_path += DIR_SEPARATOR_STR;
 full_path += p_path;
 return DirHandler(full_path);
}

bool DirHandler::Create(bool recursive)
{
 SYS_DEBUG_MEMBER(DM_FILE);

 SYS_DEBUG(DL_FILE, "To be created: '" << path << "'");

 if (IsDirectory(path)) {
    SYS_DEBUG(DL_FILE, path << " - already exists");
    return false; // not created
 }

 if (recursive) {
    size_t slash = path.find_last_of(DIR_SEPARATOR);
    if (slash != std::string::npos) {
        while (path[slash] == DIR_SEPARATOR) {
            if (!slash) {
                SYS_DEBUG(DL_FILE, " - at root");
                return false; // do not create root
            }
            --slash;
        }
        std::string base_path(path.substr(0, slash+1));
        DirHandler parent(base_path);
        parent.Create(true);
    }
 }

 SYS_DEBUG(DL_FILE, "Creating '" << path << "'...");

 if (mkdir(path.c_str(), mode) != 0) {
    throw EX::DIR_Exception() << "'" << path << "' could not be created: " << strerror(errno);
 }

 SYS_DEBUG(DL_FILE, "Ok.");

 return true; // created
}

bool DirHandler::Create(const char * sub_path, bool recursive)
{
 SYS_DEBUG_MEMBER(DM_FILE);
 DirHandler subdir(SubDir(sub_path));
 subdir.Create(recursive);
}

bool DirHandler::IsDirectory(const char * path)
{
 struct stat st;
 int result = stat(path, &st);
 if (result < 0) {
    throw EX::DIR_Exception() << "Could not stat(): '" << path << "'";
 }
 return S_ISDIR(st.st_mode);
}

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
