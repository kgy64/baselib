/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     My Generic C++ Library
 * Purpose:     Generic file handler classes for read and write
 * Author:      György Kövesdi (kgy@teledigit.eu)
 * Licence:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <errno.h>
#include <fcntl.h>

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

 myDir = path.substr(0, slash+1);

 SYS_DEBUG(DL_INFO2, "My dir: '" << myDir << "', filename: '" << myName << "'");
}

FileHandler::~FileHandler()
{
 SYS_DEBUG_MEMBER(DM_FILE);

 if (fNo > 2) {
    SYS_DEBUG(DL_INFO3, "Closing file number=" << fNo);
    ASSERT_STD(close(fNo) == 0);
 }
}

std::string FileHandler::GetFullPath(void) const
{
 std::string full_path(myDir);
 full_path += DIR_SEPARATOR_STR;
 full_path += myName;
 return full_path;
}

off_t FileHandler::Tell(void) const
{
 off_t result = lseek(fNo, 0, SEEK_CUR);
 ASSERT(result != (off_t)-1, "cannot seek");
 return result;
}

void FileHandler::Open(FILES::FileMode flag)
{
 SYS_DEBUG_MEMBER(DM_FILE);

 if (fNo >= 0) {
    // Already opened:
    throw EX::File_Error() << "Trying to re-open file, fd=" << fNo << ", name='" << myName << "'";
 }

 int open_flags = O_RDONLY;

 std::string full_path(GetFullPath());

 switch (flag) {
    case READ_ONLY:
        // Nothing to do here
        SYS_DEBUG(DL_INFO3, "Opening " << full_path << " for read-only");
    break;

    case READ_WRITE:
        open_flags = O_RDWR | O_CREAT;
        GetDirHandler()->Create(true);
        SYS_DEBUG(DL_INFO3, "Opening " << full_path << " for read-write");
    break;

    case APPEND_WRITE:
        open_flags = O_RDWR | O_APPEND | O_CREAT;
        GetDirHandler()->Create(true);
        SYS_DEBUG(DL_INFO3, "Opening " << full_path << " for append-write");
    break;
 }

 open_flags |= O_LARGEFILE;

 fNo = open(full_path.c_str(), open_flags, mode);

 if (fNo < 0) {
    throw EX::File_Error() << "Could not open '" << full_path << "': " << strerror(errno);
 }

 SYS_DEBUG(DL_INFO3, "File " << full_path << " is opened: file number=" << fNo << ", pos=" << Tell());
}

void FileHandler::OpenSpecial(FILES::FileMode flag)
{
 SYS_DEBUG_MEMBER(DM_FILE);

 if (myName != "-" || myDir != ".") {
    // Normal open:
    Open(flag);
    return;
 }

 switch (flag) {
    case READ_ONLY:
        SYS_DEBUG(DL_INFO1, "Reading from standard input");
        fNo = 0;
    break;
    case READ_WRITE:
    case APPEND_WRITE:
        SYS_DEBUG(DL_INFO1, "Writing to standard output");
        fNo = 1;
    break;
    default:
        throw EX::File_Error() << "Wrong open mode for special file";
    break;
 }
}

size_t FileHandler::Write(const void * p_data, size_t p_length)
{
 SYS_DEBUG_MEMBER(DM_FILE);

 if (fNo < 0) {
    throw EX::File_Error() << "file '" << GetFullPath() << "' not opened for writing";
 }

 if (fNo == 0) {
    throw EX::File_Error() << "Write to standard input";
 }

 if (!p_length) {
    return 0;
 }

 SYS_DEBUG(DL_INFO3, "Writing " << p_length << " bytes to position " << Tell());

 for (size_t size = p_length; size; ) {
    ssize_t result = 0;
    while (true) {
        result = write(fNo, p_data, size);
        if (result > 0) {
            break;
        }
        switch (errno) {
            case EWOULDBLOCK:
#if EAGAIN != EWOULDBLOCK
            case EAGAIN:
#endif
                BlockedIo();
            break;
            default:
                throw EX::File_Error() << "Error writing " << p_length << " bytes, fd=" << fNo << "; " << strerror(errno);
            break;
        }
    }
    size -= result;
    p_data = reinterpret_cast<const char *>(p_data) + result;
 }

 return p_length;
}

bool FileHandler::Read(void * p_data, size_t p_length)
{
 SYS_DEBUG_MEMBER(DM_FILE);

 if (fNo < 0) {
    throw EX::File_Error() << "file '" << GetFullPath() << "' not opened for reading";
 }

 if (fNo == 1 || fNo == 2) {
    throw EX::File_Error() << "Read from standard out/error";
 }

 if (!p_length) {
    return true;
 }

 char * data = (char *)p_data;

 size_t offset = 0;

do_again:;
 ssize_t result = read(fNo, (void*)(data+offset), p_length);

 if (result < 0) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
        BlockedIo();
        goto do_again;
    }
    throw EX::File_Error() << "Error reading " << p_length << " bytes, fd=" << fNo << "; " << strerror(errno);
 }

 if (result < (ssize_t)p_length) {
    if (result == 0) {
        if (offset == 0) {
            return false;
        }
        throw EX::File_EOF(offset) << "EOF reached while reading " << p_length << " bytes from '" << GetFullPath() << "', got " << offset << " (fd=" << fNo << ")";
    }
    p_length -= result;
    offset += result;
    BlockedIo();
    goto do_again;
 }

 return true;
}

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
