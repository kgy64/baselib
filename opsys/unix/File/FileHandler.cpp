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

using namespace FILES;

FileHandler::FileHandler(DirPtr & p_dir, const char * p_filename):
    myDir(p_dir),
    myName(p_filename),
    mode(DEFAULT_MODE),
    fNo(-1)
{
 SYS_DEBUG_MEMBER(DM_FILE);

 if (myName.empty()) {
    throw EX::File_Error() << "No filename given";
 }
}

FileHandler::FileHandler(const char * p_path, const char * p_name):
    myDir(new FILES::DirHandler(p_path)),
    myName(p_name),
    mode(DEFAULT_MODE),
    fNo(-1)
{
 SYS_DEBUG_MEMBER(DM_FILE);

 if (myName.empty()) {
    throw EX::File_Error() << "No filename given";
 }
}

FileHandler::FileHandler(const char * p_full_path):
    mode(DEFAULT_MODE),
    fNo(-1)
{
 SYS_DEBUG_MEMBER(DM_FILE);

 std::string path(p_full_path);

 size_t slash = path.find_last_of(DIR_SEPARATOR);
 if (slash == std::string::npos) {
    // Filename without path is given; use the current directory:
    myName = path;
    myDir.reset(new DirHandler("."));
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

 myDir.reset(new DirHandler(path.substr(0, slash)));
}

FileHandler::~FileHandler()
{
 SYS_DEBUG_MEMBER(DM_FILE);

 if (fNo >= 0) {
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
    return;
 }

 int open_flags = O_RDONLY;
 switch (flag) {
    case READ_ONLY:
    break;

    case READ_WRITE:
        open_flags = O_RDWR | O_CREAT;
        myDir->Create(true);
    break;

    case APPEND_WRITE:
        open_flags = O_RDWR | O_APPEND | O_CREAT;
        myDir->Create(true);
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
    throw EX::File_Error() << "File not opened to write " << p_length << " bytes";
 }

 if (!p_length) {
    return;
 }

do_again:;
 ssize_t result = write(fNo, p_data, p_length);
 if (result == EAGAIN) {
    BlockedIo();
    goto do_again;
 }

 if (result < 0) {
    throw EX::File_Error() << "Error writing " << p_length << " bytes, fd=" << fNo << "; " << strerror(errno);
 }

 if (result != p_length) {
    throw EX::File_Error() << "Written " << result << " bytes instead of " << p_length << " to fd " << fNo;
 }
}

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
