/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     My Generic C++ Library
 * Purpose:     
 * Author:      György Kövesdi (kgy@teledigit.eu)
 * Licence:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <string.h>

#include "DirHandler.h"
#include <File/FileMap.h>
#include <File/FileHandler.h>

SYS_DECLARE_MODULE(DM_FILE); // Note: defined in FileHandler.cpp

using namespace FILES;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                                       *
 *     class DirHandler:                                                                 *
 *                                                                                       *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

DirHandler::DirHandler(const char * p_path)
{
 SYS_DEBUG_MEMBER(DM_FILE);

 path = p_path;
 mode = DEFAULT_MODE;
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

 SYS_DEBUG(DL_INFO1, "To be created: '" << path << "'");

 if (IsExist(path) && IsDirectory(path)) {
    SYS_DEBUG(DL_INFO1, path << " - already exists");
    return false; // not created
 }

 if (recursive) {
    size_t slash = path.find_last_of(DIR_SEPARATOR);
    if (slash != std::string::npos) {
        while (path[slash] == DIR_SEPARATOR) {
            if (!slash) {
                SYS_DEBUG(DL_INFO1, " - at root");
                return false; // do not create root
            }
            --slash;
        }
        std::string base_path(path.substr(0, slash+1));
        DirHandler parent(base_path);
        parent.Create(true);
    }
 }

 SYS_DEBUG(DL_INFO1, "Creating '" << path << "'...");

 if (mkdir(path.c_str(), mode) != 0) {
    throw EX::DIR_Exception() << "'" << path << "' could not be created: " << strerror(errno);
 }

 SYS_DEBUG(DL_INFO1, "Ok.");

 return true; // created
}

bool DirHandler::Create(const char * sub_path, bool recursive)
{
 SYS_DEBUG_MEMBER(DM_FILE);
 DirHandler subdir(SubDir(sub_path));
 return subdir.Create(recursive);
}

bool DirHandler::IsExist(const char * path)
{
 struct stat st;
 int result = stat(path, &st);
 if (result < 0) {
    if (errno == ENOENT) {
        return false;
    }
    throw EX::DIR_Exception() << "Could not stat() '" << path << "': " << strerror(errno);
 }
 return true;
}

bool DirHandler::IsDirectory(const char * path)
{
 struct stat st;
 int result = stat(path, &st);
 ASSERT(result==0, "Could not stat() '" << path << "': " << strerror(errno));

 return S_ISDIR(st.st_mode);
}

bool DirHandler::IsFile(const char * path, bool p_any)
{
 struct stat st;
 int result = stat(path, &st);
 ASSERT(result==0, "Could not stat() '" << path << "': " << strerror(errno));

 if (S_ISREG(st.st_mode)) {
    return true;
 }

 if (!p_any) {
    return false;
 }

 return S_ISLNK(st.st_mode) || S_ISCHR(st.st_mode) || S_ISBLK(st.st_mode) || S_ISFIFO(st.st_mode) || S_ISSOCK(st.st_mode);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                                       *
 *     class DirHandler::iterator:                                                       *
 *                                                                                       *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void DirHandler::iterator::_iterator(const char * p_parent)
{
 SYS_DEBUG_MEMBER(DM_FILE);

 myPath = p_parent;
 myDir = opendir(myPath.c_str());
 actualEntry = NULL;

 ASSERT_DBG(myDir, "Could not open '" << myPath << "' for iteration: " << strerror(errno));
 ++*this; // Step to the first entry
}

DirHandler::iterator::~iterator()
{
 SYS_DEBUG_MEMBER(DM_FILE);

 if (myDir) {
    ASSERT_DBG(!closedir(myDir), "Could not close directory iterator");
 }
}

std::string DirHandler::iterator::Name(void) const
{
 SYS_DEBUG_MEMBER(DM_FILE);

 if (!actualEntry) {
    return "";
 }

 return actualEntry->d_name;
}

std::string DirHandler::iterator::Pathname(void) const
{
 SYS_DEBUG_MEMBER(DM_FILE);

 if (!actualEntry) {
    return "";
 }

 return myPath + DIR_SEPARATOR_STR + actualEntry->d_name;
}

DirHandler::iterator & DirHandler::iterator::operator++()
{
 SYS_DEBUG_MEMBER(DM_FILE);

 do {
    int result = readdir_r(myDir, &myEntry, &actualEntry);
    ASSERT_DBG(!result, "Could not iterate on '" << myPath << "': " << strerror(result));
 } while (actualEntry && actualEntry->d_name[0] == '.'); // Skip '.' '..' and hidden files

 SYS_DEBUG(DL_INFO1, "Current entry: '" << (actualEntry ? actualEntry->d_name : "") << "'");

 return *this;
}

DirHandler::iterator DirHandler::iterator::operator*() const
{
 SYS_DEBUG_MEMBER(DM_FILE);

 return DirHandler::iterator(Pathname());
}

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
