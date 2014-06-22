#define _BSD_SOURCE

#include "FileMap.h"
#include "Exceptions/Exceptions.h"
#include "International/International.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

SYS_DEFINE_MODULE(DM_FILE);

using namespace FILES;

FileMap::FileMap(const char * name, OpenMode mode, size_t p_size):
    mapped(NULL),
    ende(NULL),
    size(0)
{
 SYS_DEBUG_MEMBER(DM_FILE);

 myMode = mode;

 int open_mode = O_RDONLY;
 int map_prot = PROT_READ;
 int map_mode = MAP_PRIVATE;

 switch (myMode) {
    case Read_Unsafe:
    case Read_Only:
        open_mode = O_RDONLY;
        map_prot = PROT_READ;
        map_mode = MAP_PRIVATE;
        SYS_DEBUG(DL_INFO1, "Opening '" << name << "' in read-only...");
    break;
    case Read_Write:
        open_mode = O_RDWR | O_CREAT;
        map_prot = PROT_READ | PROT_WRITE;
        map_mode = MAP_SHARED;
        SYS_DEBUG(DL_INFO1, "Opening '" << name << "' in read/write...");
    break;
    default:
        ASSERT(false, "File '" << name << "' is opened in invalid mode: " << (int)myMode);
    break;
 }

 fd = open(name, open_mode, 0644);

 if (myMode == Read_Unsafe) {
    if (fd < 0) {
        SYS_DEBUG(DL_INFO1, "File '" << name << "' does not exist.");
        return;
    }
 } else {
    ASSERT_STRERROR(fd >= 0, "File '" << name << "' could not be opened: ");
 }

 if (p_size) {
    size = p_size;
    ftruncate(fd, size);
 } else {
    struct stat sb;
    int result = fstat(fd, &sb);
    ASSERT_STD_ERRNO(result == 0, errno);
    size = sb.st_size;
 }

 if (size > 0) {
    mapped = mmap(NULL, size, map_prot, map_mode, fd, 0);
    ASSERT(mapped != MAP_FAILED, "File '" << name << "' could not be mapped.");
    ende = reinterpret_cast<char*>(mapped) + size;
    SYS_DEBUG(DL_INFO2, "File mapped from " << mapped << " to " << ende);
 }
}

FileMap::FileMap(FileMap & other)
{
 fd = other.fd;
 mapped = other.mapped;
 ende = other.ende;
 size = other.size;
 myMode = other.myMode;

 other.mapped = 0;
 other.fd = -1;
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

void FileMap::Advise(AdviseMode mode)
{
 int mAdvise = 0;
 switch (mode) {
    case Adv_Normal:
    break;
    case Adv_Random:
    break;
    case Adv_Sequential:
    break;
    case Adv_Willneed:
    break;
    case Adv_Dontneed:
    break;
    default:
        ASSERT(false, "Using wrong advice: " << (int)mode);
    break;
 }

 ASSERT_STRERROR(madvise(mapped, size, mAdvise) == 0, "madvise() failed");
}

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
