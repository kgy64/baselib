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

 SYS_DEBUG(DL_INFO1, "Opening '" << name << "' ...");

 int open_mode = O_RDONLY;
 int map_prot = PROT_READ;

 switch (mode) {
    case Read_Only:
        open_mode = O_RDONLY;
        map_prot = PROT_READ;
    break;
    case Read_Write:
        open_mode = O_RDWR;
        map_prot = PROT_READ | PROT_WRITE;
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
    mapped = mmap(NULL, size, map_prot, MAP_PRIVATE, fd, 0);
    ASSERT(mapped != MAP_FAILED, "File '" << name << "' could not be mapped.");
    ende = (void*)((char*)mapped + size);
 }
}

FileMap::~FileMap()
{
 SYS_DEBUG_MEMBER(DM_FILE);

 if (mapped && mapped != MAP_FAILED) {
    munmap(mapped, size);
 }

 if (fd >= 0) {
    close(fd);
 }
}

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
