#include "SerialPort.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <alloca.h>

#include <string>

SYS_DEFINE_MODULE(DM_SERIAL);

using namespace SYS;

SerialPort::SerialPort(void)
{
 SYS_DEBUG_MEMBER(DM_SERIAL);
}

SerialPort::~SerialPort()
{
 SYS_DEBUG_MEMBER(DM_SERIAL);
}

bool SerialPort::Open(const char * DeviceName, int baud)
{
 SYS_DEBUG_MEMBER(DM_SERIAL);

 std::string name(DeviceName);

 for (int i = 0; i < 100; ++i) {
    struct stat file_stat;

    if (lstat(name.c_str(), &file_stat) < 0) {
        SYS_DEBUG(DL_ERROR, "Cannot stat() file '" << name << "'");
        return false;
    }

    if (S_ISDIR(file_stat.st_mode)) {
        SYS_DEBUG(DL_ERROR, "The given file '" << name << "' is a directory.");
        return false;
    }

    if (!S_ISLNK(file_stat.st_mode)) {
        is_device = S_ISCHR(file_stat.st_mode);
        fd = open(name.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
        if (fd < 0) {
            SYS_DEBUG(DL_ERROR, "Error opening serial port (or file) '" << name << "'");
            return false;
        }
        if (is_device) {
            struct termios termio;
            if (tcgetattr(fd, &termio) < 0) {
                SYS_DEBUG(DL_ERROR, "tcgetattr() failed on '" << name << "'");
                Close();
                return false;
            }

            // We need raw bytes:
            cfmakeraw(&termio);

            cfsetispeed(&termio, baud);
            cfsetospeed(&termio, baud);

            termio.c_iflag |=  BRKINT;
            termio.c_iflag &=~ ICRNL;
            termio.c_iflag &=~ IGNBRK;
            termio.c_iflag &=~ IGNCR;
            termio.c_iflag &=~ IGNPAR;  // TODO
            termio.c_iflag &=~ INLCR;
            termio.c_iflag &=~ INPCK;   // TODO
            termio.c_iflag &=~ ISTRIP;
            termio.c_iflag &=~ IXOFF;
            termio.c_iflag &=~ IXON;
            termio.c_iflag &=~ PARMRK;  // TODO

            termio.c_oflag &=~ OPOST;

            termio.c_cflag |=  CLOCAL;
            termio.c_cflag |=  CREAD;
            termio.c_cflag &=~ CSIZE;
            termio.c_cflag |=  CS8;
            termio.c_cflag &=~ CSTOPB;
            termio.c_cflag &=~ HUPCL;
            termio.c_cflag &=~ PARENB;

            termio.c_lflag &=~ (ECHO | ECHOE | ECHOK | ECHONL);
            termio.c_lflag &=~ ICANON;
            termio.c_lflag &=~ IEXTEN;
            termio.c_lflag |=  ISIG;
            termio.c_lflag |=  NOFLSH;
            termio.c_lflag |=  TOSTOP;

            // Throw away any old data
            tcflush(fd, TCIOFLUSH);

            // Set the serial port immediately
            tcsetattr(fd, TCSANOW, &termio);
        }

        SYS_DEBUG(DL_INFO1, (is_device ? "Device '" : "Input file '") << name << "' is opened.");
        return true;
    }

    // This is a symbolic link. Follow it to determine if it refers to a device or regular file:
    char * newname = (char*)alloca(file_stat.st_size+1);
    ssize_t r = readlink(name.c_str(), newname, file_stat.st_size+1);
    if (r < 0) {
        SYS_DEBUG(DL_ERROR, "Cannot follow symbolic link '" << name << "'");
        return false;
    }

    if (r > file_stat.st_size) {
        SYS_DEBUG(DL_ERROR, "Symbolic link is changed during operation.");
        return false;
    }

    newname[r] = '\0';
    SYS_DEBUG(DL_INFO1, "Following symbolic link: " << name << " -> " << newname);
    name = newname;
 }

 SYS_DEBUG(DL_INFO1, "Symbolic link is too deep (>100)");

 return false;
}

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
