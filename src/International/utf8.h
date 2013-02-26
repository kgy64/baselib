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

#include <boost/scoped_ptr.hpp>
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

        inline size_t length(void) const
        {
            return myLength;
        }

        inline bool empty(void) const
        {
            return myLength == 0;
        }

        inline const WChar * ToWChar(size_t p_start = 0) const
        {
            if (p_start >= length()) {
                return L"";
            }
            return myStr.get() + p_start;
        }

        std::wstring ToString(size_t p_start = 0) const
        {
            return ToWChar(p_start);
        }

        inline WChar operator[](size_t p_index) const
        {
            return *ToWChar(p_index);
        }

        static WChar GetChar(const char *& p_str);

     private:
        SYS_DEFINE_CLASS_NAME("UTF8::ToWstring");

        size_t myLength;

        const std::auto_ptr<WChar> myStr;

        static WChar GetContinuationChar(const char *& p_str);
    };

    class WideStringIterator: public ToWstring
    {
     public:
        inline WideStringIterator(const char * p_str):
            UTF8::ToWstring(p_str),
            myPosition(0)
        {
        }

        inline UTF8::WChar operator*() const
        {
            return *ToWChar(myPosition);
        }

        inline WideStringIterator & operator++()
        {
            ++myPosition;
            return *this;
        }

        inline const WChar * Remaining(void) const
        {
            return ToWChar(myPosition);
        }

     protected:
        size_t myPosition;

     private:
        SYS_DEFINE_CLASS_NAME("UTF8::WideStringIterator");

    }; // class WideStringIterator

    class FromWstring
    {
        // Some constructor-like functions, to be compatible with older
        // compilers without c++11 support (instead of delegating these
        // constructors):

        void _FromWstring(const WChar * p_str);

     public:
        inline FromWstring(const WChar * p_str)
        {
            _FromWstring(p_str);
        }

        inline FromWstring(const std::wstring & p_st)
        {
            _FromWstring(p_st.c_str());
        }

        /// Returns the length of the UTF8 representation of a Unicode character
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

        /// Returns the string length in UTF8 representation
        inline static size_t CalculateLength(const WChar * p_char)
        {
            size_t result = 0;
            while (*p_char) result += CalculateLength(*p_char++);
            return result;
        }

        inline size_t length(void) const
        {
            return myLength;
        }

        inline bool empty(void) const
        {
            return myLength == 0;
        }

        inline const char * ToChar(size_t p_start = 0) const
        {
            if (p_start >= length()) {
                return "";
            }
            return myStr.get() + p_start;
        }

        inline std::string ToString(size_t p_start = 0) const
        {
            return ToChar(p_start);
        }

        inline char operator[](size_t p_index) const
        {
            return *ToChar(p_index);
        }

        static void GetChar(WChar p_char, char *& p_result);

     private:
        SYS_DEFINE_CLASS_NAME("UTF8::FromWstring");

        size_t myLength;

        boost::scoped_ptr<char> myStr;
    };
};

std::ostream & operator<<(std::ostream & os, const UTF8::WChar * st);
std::ostream & operator<<(std::ostream & os, UTF8::WChar ch);

#endif /* __SRC_INTERNATIONAL_UTF8_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
