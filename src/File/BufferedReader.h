/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     My Generic C++ Library
 * Purpose:     
 * Author:      György Kövesdi (kgy@teledigit.eu)
 * Licence:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __BASELIB_SRC_FILE_BUFFEREDREADER_H_INCLUDED__
#define __BASELIB_SRC_FILE_BUFFEREDREADER_H_INCLUDED__

#include <File/FileHandler.h>

namespace FILES
{
    /// File handler class optimized for reading byte-by-byte
    template <unsigned BufferSize = 4096>
    class BufferedReader
    {
     public:
        inline BufferedReader(const char * p_path):
            myFile(p_path),
            myBufferPosition(0),
            myLastChar(0)
        {
        }

        inline BufferedReader(const std::string & p_path):
            myFile(p_path),
            myBufferPosition(0),
            myLastChar(0)
        {
        }

        void Open(void)
        {
            myFile.Open(FILES::READ_ONLY);
        }

        void OpenSpecial(void)
        {
            myFile.OpenSpecial(FILES::READ_ONLY);
        }

        inline int ReadByte(void)
        {
            return (myBufferPosition < myLastChar) ? myBuffer[++myBufferPosition] : ReadBlock();
        }

     private:
        SYS_DEFINE_CLASS_NAME("FILES::BufferedReader");

        int ReadBlock(void)
        {
            SYS_DEBUG_MEMBER(DM_FILE);
            myBufferPosition = 0;
            SYS_DEBUG(DL_VERBOSE, "Out of data, reading...");
            try {
                if (!myFile.Read(myBuffer)) {
                    SYS_DEBUG(DL_INFO1, "Really EOF");
                    return EOF;
                }
                myLastChar = sizeof(myBuffer) - 1;
            } catch (EX::File_EOF & ex) {
                SYS_DEBUG(DL_INFO1, " - EOF reached");
                myLastChar = ex.GetBytes() - 1;
            }
            SYS_DEBUG(DL_INFO1, " - Got " << myLastChar+1 << " bytes.");
            return myBuffer[0];
        }

        FileHandler myFile;

        uint8_t myBuffer[BufferSize];

        /// Points to the last element read
        int myBufferPosition;

        /// Points to the last valid data in BufferedReader::myBuffer
        int myLastChar;
    };
} // namespace FILES

#endif /* __BASELIB_SRC_FILE_BUFFEREDREADER_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
