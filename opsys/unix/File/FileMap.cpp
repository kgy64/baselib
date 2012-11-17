#include "FileMap.h"
#include "Exceptions/ICExceptions.h"
#include "International/International.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

FileMap::FileMap(const char * name, OpenMode mode)
{
 SYS_DEBUG_MEMBER(DM_FILE);

 SYS_DEBUG(DL_FILE, "Opening '" << name << "' ...");

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
 if (fd < 0) {
    SYS_DEBUG(DL_FILE, "File '" << name << "' could not be opened.");
    throw EX::Problem(_I("File cannot be opened"));
 }
 struct stat sb;
 if (fstat(fd, &sb) < 0) {
    SYS_DEBUG(DL_FILE, "Stat error on file '" << name << "'");
    throw EX::Problem(_I("File 'stat()' error"));
 }
 size = sb.st_size;
 if (size == 0) {
    mapped = NULL;
 } else {
    mapped = mmap(NULL, size, map_prot, MAP_PRIVATE, fd, 0);
    if (mapped == MAP_FAILED) {
        SYS_DEBUG(DL_FILE, "File '" << name << "' could not be mapped.");
        throw EX::Problem(_I("mmap() error"));
    }
 }
 ende = (void*)((char*)mapped + size);
}

FileMap::~FileMap()
{
 SYS_DEBUG_MEMBER(DM_FILE);

 if (mapped && mapped != MAP_FAILED)
    munmap(mapped, size);
 if (fd >= 0)
    close(fd);
}

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
