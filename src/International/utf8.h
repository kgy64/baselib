/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     
 * Purpose:     
 * Author:      
 * Licence:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __SRC_INTERNATIONAL_UTF8_H_INCLUDED__
#define __SRC_INTERNATIONAL_UTF8_H_INCLUDED__

#include <memory>
#include <string>

#include <Exceptions/ICExceptions.h>
#include <Debug/Debug.h>

namespace UTF8
{
    DEFINE_EXCEPTION(UTF8_Conversion, "UTF8 Conversion", ::EX::Error);

    typedef wchar_t WChar;

    enum BitMasks
    {
        CONTINUATION_MASK   =   0x0000003f,
        CONTINUATION_ENCODE =   0x00000080,
        ONE_BYTE_MASK       =   0x0000007f,
        TWO_BYTE_MASK       =   0x000007ff,
        TWO_BYTE_ENCODE     =   0x000000c0,
        THREE_BYTE_MASK     =   0x0000ffff,
        THREE_BYTE_ENCODE   =   0x000000e0,
        FOUR_BYTE_MASK      =   0x001fffff,
        FOUR_BYTE_ENCODE    =   0x000000f0,
    };

    class ToWstring
    {
     public:
        ToWstring(const char * p_str);

        inline const WChar * ToWChar(void) const
        {
            return myStr.get();
        }

        inline size_t length(void) const
        {
            return myLength;
        }

        inline bool empty(void) const
        {
            return myLength == 0;
        }

        std::wstring ToString(void) const
        {
            return ToWChar();
        }

     private:
        SYS_DEFINE_CLASS_NAME("UTF8::ToWstring");

        size_t myLength;

        const std::auto_ptr<WChar> myStr;

        static WChar GetChar(const char *& p_str);
        static WChar GetContinuationChar(const char *& p_str);
    };

    class FromWstring
    {
     public:
        FromWstring(const WChar * p_str);

        inline FromWstring(const std::wstring & p_st):
            FromWstring(p_st.c_str())
        {
        }

        inline static size_t CalculateLength(WChar p_char)
        {
            if ((p_char & ~ONE_BYTE_MASK) == 0)
                return 1;
            if ((p_char & ~TWO_BYTE_MASK) == 0)
                return 2;
            if ((p_char & ~THREE_BYTE_MASK) == 0)
                return 3;
            if ((p_char & ~FOUR_BYTE_MASK) == 0)
                return 4;
            throw UTF8_Conversion() << "Invalid wide character value: " << std::hex << (int)p_char;
        }

        inline static size_t CalculateLength(const WChar * p_char)
        {
            size_t result = 0;
            while (*p_char) result += CalculateLength(*p_char++);
            return result;
        }

        inline const char * ToChar(void) const
        {
            return myStr.get();
        }

        inline size_t length(void) const
        {
            return myLength;
        }

        inline bool empty(void) const
        {
            return myLength == 0;
        }

        std::string ToString(void) const
        {
            return ToChar();
        }

     private:
        SYS_DEFINE_CLASS_NAME("UTF8::FromWstring");

        size_t myLength;

        const std::auto_ptr<char> myStr;

        static void GetChar(WChar p_char, char *& p_result);
    };
};

#endif /* __SRC_INTERNATIONAL_UTF8_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
