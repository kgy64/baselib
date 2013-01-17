#include "FileMap.h"
#include "Exceptions/ICExceptions.h"
#include "International/International.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

SYS_DEFINE_MODULE(DM_FILE);

using namespace FILES;

void FileMap::_FileMap(const char * name, OpenMode mode)
{
 SYS_DEBUG_MEMBER(DM_FILE);

 myMode = mode;

 int open_mode = O_RDONLY;
 int map_prot = PROT_READ;
 int map_mode = MAP_PRIVATE;

 switch (myMode) {
    case Read_Only:
        open_mode = O_RDONLY;
        map_prot = PROT_READ;
        map_mode = MAP_PRIVATE;
        SYS_DEBUG(DL_INFO1, "Opening '" << name << "' in read-only...");
    break;
    case Read_Write:
        open_mode = O_RDWR;
        map_prot = PROT_READ | PROT_WRITE;
        map_mode = MAP_SHARED;
        SYS_DEBUG(DL_INFO1, "Opening '" << name << "' in read/write...");
    break;
    default:
        ASSERT(false, "File '" << name << "' is opened in invalid mode: " << (int)myMode);
    break;
 }

 fd = open(name, open_mode);
 ASSERT(fd >= 0, "File '" << name << "' could not be opened.");

 struct stat sb;

 {
    int result = fstat(fd, &sb);
    ASSERT(result == 0, "Stat error on file '" << name << "'");
 }

 size = sb.st_size;
 if (size == 0) {
    mapped = NULL;
    ende = NULL;
 } else {
    mapped = mmap(NULL, size, map_prot, map_mode, fd, 0);
    ASSERT(mapped != MAP_FAILED, "File '" << name << "' could not be mapped.");
    ende = (void*)((char*)mapped + size);
 }
}

FileMap::~FileMap()
{
 SYS_DEBUG_MEMBER(DM_FILE);

 if (mapped && mapped != MAP_FAILED) {
    if (myMode == Read_Write) {
        SYS_DEBUG(DL_VERBOSE, "Opened for R/W: syncing...");
        int result = msync(mapped, size, MS_ASYNC | MS_INVALIDATE);
        ASSERT(result == 0, "msync() failed: " << strerror(errno));
    }
    SYS_DEBUG(DL_VERBOSE, "Unmapping...");
    munmap(mapped, size);
 }

 if (fd >= 0) {
    SYS_DEBUG(DL_VERBOSE, "Closing fd=" << fd);
    close(fd);
 }
}

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
