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

 switch (myMode & _OPEN_MASK) {
    case Read_Unsafe:
    case Read_Only:
        open_mode = O_RDONLY;
        map_prot = PROT_READ;
        SYS_DEBUG(DL_INFO1, "Opening '" << name << "' in read-only...");
    break;
    case Read_Write:
        open_mode = O_RDWR | O_CREAT;
        map_prot = PROT_READ | PROT_WRITE;
        SYS_DEBUG(DL_INFO1, "Opening '" << name << "' in read/write...");
    break;
    default:
        ASSERT(false, "File '" << name << "' is opened in invalid mode: " << (int)myMode);
    break;
 }

 map_mode = myMode & Map_Shared ? MAP_PRIVATE : MAP_SHARED;

 if (myMode & Map_Nonblock) {
    map_mode |= MAP_NONBLOCK;
 }

 fd = open(name, open_mode, 0644);

 if ((myMode & _OPEN_MASK) == Read_Unsafe) {
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

FileMap::FileMap(FileMap && other)
{
 SYS_DEBUG_MEMBER(DM_FILE);

 fd = other.fd;
 mapped = other.mapped;
 ende = other.ende;
 size = other.size;
 myMode = other.myMode;

 other.mapped = 0;
 other.ende = 0;
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

/// Advises the kernel about how to handle paging
void FileMap::Advise(AdviseMode mode)
{
 int mAdvise = MADV_NORMAL;
 switch (mode) {
    case Adv_Normal:
        mAdvise = MADV_NORMAL;
    break;
    case Adv_Random:
        mAdvise = MADV_RANDOM;
    break;
    case Adv_Sequential:
        mAdvise = MADV_SEQUENTIAL;
    break;
    case Adv_Willneed:
        mAdvise = MADV_WILLNEED;
    break;
    case Adv_Dontneed:
        mAdvise = MADV_DONTNEED;
    break;
    default:
        ASSERT(false, "Using wrong advice: " << (int)mode);
    break;
 }

 ASSERT_STRERROR(madvise(mapped, size, mAdvise) == 0, "madvise() failed");
}

void FileMap::Sync(bool wait)
{
 ASSERT_STRERROR(msync(mapped, size, wait ? MS_SYNC : MS_ASYNC) == 0, "msync() failed");
}

void FileMap::Populate(void)
{
 ASSERT_STRERROR(msync(mapped, size, MS_INVALIDATE) == 0, "msync() failed");
}

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
