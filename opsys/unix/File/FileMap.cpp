#define _DEFAULT_SOURCE

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <File/Decode.h>
#include <Exceptions/Exceptions.h>
#include <International/International.h>

#include "FileMap.h"

SYS_DEFINE_MODULE(DM_FILE);

using namespace FILES;

FileMap::FileMap(const char * name, OpenMode mode, size_t p_size):
    fd(-1),
    mapped(nullptr),
    ende(nullptr),
    size(0)
{
 SYS_DEBUG_MEMBER(DM_FILE);

 try {
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

    std::string decoded_name = FILES::DecodeName(name);
    fd = open(decoded_name.c_str(), open_mode, 0644);

    if ((myMode & _OPEN_MASK) == Read_Unsafe) {
        if (fd < 0) {
            SYS_DEBUG(DL_INFO1, "File '" << decoded_name << "' does not exist.");
            return;
        }
    } else {
        ASSERT_STRERROR(fd >= 0, "File '" << decoded_name << "' could not be opened: ");
    }

    {
        struct stat sb;
        ASSERT_STRERROR(!fstat(fd, &sb), "fstat('" << decoded_name << "') failed: ");
        if (p_size) {
            size = p_size;
            // Truncate only the regular files:
            if (sb.st_mode & S_IFREG && mode & Map_Truncate) {
                ASSERT_STRERROR(!ftruncate(fd, size), "ftruncate('" << decoded_name << "', " << size << ") failed: ");
            }
        } else {
            size = sb.st_size;
        }
    }

    if (size > 0) {
        mapped = mmap(nullptr, size, map_prot, map_mode, fd, 0);
        ASSERT_STRERROR(mapped != MAP_FAILED, "File '" << decoded_name << "' (fd=" << fd << ") could not be mapped: ");
        ende = reinterpret_cast<char*>(mapped) + size;
        SYS_DEBUG(DL_INFO2, "File '" << decoded_name << "' mapped from " << mapped << " to " << ende);
    }

    return; // Everything was OK.

 } catch (EX::BaseException & ex) {
    std::cerr << "ERROR: Mapping '" << name << "' is failed because " << ex.what() << std::endl;
 } catch (std::exception & ex) {
    std::cerr << "ERROR: Mapping '" << name << "' is failed: " << ex.what() << std::endl;
 } catch (...) {
    std::cerr << "ERROR: Mapping '" << name << "' is failed due to unknown exception." << std::endl;
 }

 // Unmap if necessary:
 if (mapped && mapped != MAP_FAILED) {
    munmap(mapped, size);
    mapped = nullptr;
 }

 size = 0;

 // Close if necessary:
 if (fd >= 0) {
    close(fd);
    fd = -1;
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

 try {
    if (mapped && mapped != MAP_FAILED) {
        if (myMode == Read_Write) {
            SYS_DEBUG(DL_VERBOSE, "Opened for R/W: syncing...");
            Sync(false);
        }
        SYS_DEBUG(DL_VERBOSE, "Unmapping...");
        munmap(mapped, size);
    }
 } catch(std::exception & ex) {
    std::cerr << "ERROR: could not unmap memory: " << ex.what() << ", fd=" << fd << std::endl;
 } catch (...) {
    std::cerr << "ERROR: could not unmap memory due to unknown reason, fd=" << fd << std::endl;
 }

 if (fd >= 0) {
    SYS_DEBUG(DL_VERBOSE, "Closing fd=" << fd);
    close(fd);
    fd = -1;
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

bool FileMap::isOk(void) const
{
 return mapped && mapped != MAP_FAILED;
}

void FileMap::Sync(bool wait)
{
 ASSERT_STRERROR(msync(mapped, size, (wait ? MS_SYNC : MS_ASYNC) | MS_INVALIDATE) == 0, "msync() failed: ");
}

void FileMap::Populate(void)
{
 ASSERT_STRERROR(msync(mapped, size, MS_INVALIDATE) == 0, "msync() failed: ");
}

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
