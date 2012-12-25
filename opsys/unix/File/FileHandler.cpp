/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     
 * Purpose:     
 * Author:      
 * Licence:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <Exceptions/ICExceptions.h>

#include "FileHandler.h"

SYS_DECLARE_MODULE(DM_FILE);

using namespace FILES;

FileHandler::FileHandler(const DirHandler & p_dir, const char * p_filename):
    FileHandler()
{
 SYS_DEBUG_MEMBER(DM_FILE);

 myDir = p_dir.GetPath();
 myName = p_filename;

 if (myName.empty()) {
    throw EX::File_Error() << "No filename given";
 }
}

FileHandler::FileHandler(const char * p_path, const char * p_name):
    FileHandler()
{
 SYS_DEBUG_MEMBER(DM_FILE);

 myDir = p_path;
 myName = p_name;

 if (myName.empty()) {
    throw EX::File_Error() << "No filename given";
 }
}

FileHandler::FileHandler(const char * p_full_path):
    FileHandler()
{
 SYS_DEBUG_MEMBER(DM_FILE);

 std::string path(p_full_path);

 size_t slash = path.find_last_of(DIR_SEPARATOR);
 if (slash == std::string::npos) {
    // Filename without path is given; use the current directory:
    myName = path;
    myDir = ".";
    return;
 }

 myName = path.substr(slash+1);

 if (myName.empty()) {
    throw EX::File_Error() << "No filename in path: '" << p_full_path << "'";
 }

 // Eat repeated slashes:
 while (slash > 1 && path[slash] == DIR_SEPARATOR) {
    --slash;
 }

 myDir = path.substr(0, slash);
}

FileHandler::~FileHandler()
{
 SYS_DEBUG_MEMBER(DM_FILE);

 if (fNo > 2) {
    if (close(fNo) < 0) {
        throw EX::File_Error() << "Could not close file number " << fNo;
    }
 }
}

void FileHandler::Open(FILES::FileMode flag)
{
 SYS_DEBUG_MEMBER(DM_FILE);

 if (fNo >= 0) {
    // Already opened:
    throw EX::File_Error() << "Trying to re-open file, fd=" << fNo;
 }

 int open_flags = O_RDONLY;

 switch (flag) {
    case READ_ONLY:
        // Nothing to do here
    break;

    case READ_WRITE:
        open_flags = O_RDWR | O_CREAT;
        GetDirHandler()->Create(true);
    break;

    case APPEND_WRITE:
        open_flags = O_RDWR | O_APPEND | O_CREAT;
        GetDirHandler()->Create(true);
    break;
 }

 std::string full_path(GetFullPath());

 fNo = open(full_path.c_str(), open_flags, mode);

 if (fNo < 0) {
    throw EX::File_Error() << "Could not open '" << full_path << "': " << strerror(errno);
 }
}

void FileHandler::Write(const void * p_data, size_t p_length)
{
 SYS_DEBUG_MEMBER(DM_FILE);

 if (fNo < 0) {
    throw EX::File_Error() << "File not opened to write";
 }

 if (fNo == 0) {
    throw EX::File_Error() << "Write to standard input";
 }

 if (!p_length) {
    return;
 }

do_again:;
 ssize_t result = write(fNo, p_data, p_length);
 if (result == -1 && errno == EAGAIN) {
    BlockedIo();
    goto do_again;
 }

 if (result < 0) {
    throw EX::File_Error() << "Error writing " << p_length << " bytes, fd=" << fNo << "; " << strerror(errno);
 }

 if (result != (int)p_length) {
    throw EX::File_Error() << "Written " << result << " bytes instead of " << p_length << " to fd " << fNo;
 }
}

bool FileHandler::Remove(void)
{
 std::string full_path(GetFullPath());
 struct stat st;
 int result = stat(full_path.c_str(), &st);
 if (result < 0) {
    switch (errno) {
        case ENOENT:
            // Does not exist: not an error here
            return false;
        break;
    }
    throw EX::File_Error() << "Could not stat() '" << full_path << "'";
 }

 result = unlink(full_path.c_str());
 if (result < 0) {
    throw EX::File_Error() << "Could not unlink() '" << full_path << "'";
 }

 return true;
}

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
