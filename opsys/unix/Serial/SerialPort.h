#ifndef __SERIALPORT_H__
#define __SERIALPORT_H__

#include <Debug/Debug.h>

#include <stdint.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>

namespace SYS
{
    class SerialPort
    {
     public:
        SerialPort(void);
        virtual ~SerialPort();

        bool Open(const char * DeviceName, int baud = 9600);

        void Close(void)
        {
            close(fd);
            fd = -1;
        }

        inline int Read(void * Buffer, int Size)
        {
            int status = read(fd, Buffer, Size);
            if (IsDevice()) {
                if (status < 0 && (errno == EAGAIN || errno == EINTR))
                    status = 0;
            } else {
                if (status == 0) // EOF
                    status = -1;
            }
            return status;
        }

        int ReadLine(char * buffer, int size, int usTimeout, bool removeCRLF = false);

        inline int Write(const void * Buffer, int Size)
        {
            int status = write(fd, Buffer, Size);
            if (status < 0 && (errno == EAGAIN || errno == EINTR))
                status = 0;
            return status;
        }

        inline void Flush(void)
        {
            tcflush(fd, TCIOFLUSH);
        }

        inline bool IsValid(void) const
        {
            return fd >= 0;
        }

        inline bool IsDevice(void) const
        {
            return is_device;
        }

     protected:
        int fd;

        bool is_device;

        virtual void Sleep(void)
        {
        }

     private:
        SYS_DEFINE_CLASS_NAME("SerialPort");
    };
}

#endif /* __SERIALPORT_H__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
