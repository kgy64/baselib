/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     
 * Purpose:     
 * Author:      
 * Licence:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "utf8.h"

SYS_DEFINE_MODULE(DM_UTF8);

using namespace UTF8;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                                       *
 *       class ToWstring:                                                                *
 *                                                                                       *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/// Convert a UTF8 string to Unicode
ToWstring::ToWstring(const char * p_str):
    myLength(strlen(p_str)),
    myStr(new WChar[myLength]) // Note: it can be more than necessary
{
 SYS_DEBUG_MEMBER(DM_UTF8);

 WChar wc;
 size_t i;
 for (i = 0; (wc = GetChar(p_str)) != 0; ++i) {
    myStr.get()[i] = wc;
 }
 myStr.get()[(myLength = i)] = 0;
}

/// Reads a continuation character within the UTF8 sequence
WChar ToWstring::GetContinuationChar(const char *& p_str)
{
 SYS_DEBUG_STATIC(DM_UTF8);

 WChar continuation = static_cast<WChar>(*reinterpret_cast<const unsigned char *>(p_str++));
 if (continuation < 0x80 || continuation >= 0xc0) {
    throw UTF8_Conversion() << "Wrong continuation char";
 }
 return continuation & 0x3f;
}

/// Convert one UTF8 character to Unicode
WChar ToWstring::GetChar(const char *& p_str)
{
 SYS_DEBUG_STATIC(DM_UTF8);

 WChar actual = static_cast<WChar>(*reinterpret_cast<const unsigned char *>(p_str++));

 if (actual >= 0 && actual <= 0x7f) {
    // Simple ASCII character:
    SYS_DEBUG(DL_VERBOSE, " - Simple ASCII char: " << std::hex << actual);
    return actual;
 }

 switch (actual & 0xf0) {
    case 0xc0:
        if (actual < 0xc2) {
            break;
        }
        // No break!
    case 0xd0:
        {
            // 2-byte sequence (0x0080...0x07ff):
            int cont_1 = GetContinuationChar(p_str);
            int result = ((actual & 0x1f) << 6) | cont_1;
            SYS_DEBUG(DL_VERBOSE, " - 2-byte sequence: " << std::hex << actual << "-" << cont_1 << "=" << result);
            return result;
        }
    break;
    case 0xe0:
        {
            // 3-byte sequence (0x0800...0xffff):
            int cont_1 = GetContinuationChar(p_str);
            int cont_2 = GetContinuationChar(p_str);
            int result = cont_2 | ((cont_1 | ((actual & 0x0f) << 6)) << 6);
            SYS_DEBUG(DL_VERBOSE, " - 3-byte sequence: " << std::hex << actual << "-" << cont_1 << "-" << cont_2 << "=" << result);
            return result;
        }
    break;
    case 0xf0:
        {
            // 4-byte sequence (>=0x10000):
            if (actual >= 0xf5) {
                // Currently, these are not used (invalid):
                break;
            }
            int cont_1 = GetContinuationChar(p_str);
            int cont_2 = GetContinuationChar(p_str);
            int cont_3 = GetContinuationChar(p_str);
            int result = cont_3 | ((cont_2 | ((cont_1 | ((actual & 0x0f) << 6)) << 6)) << 6);
            SYS_DEBUG(DL_VERBOSE, " - 4-byte sequence: " << std::hex << actual << "-" << cont_1 << "-" << cont_2 << "-" << cont_3 << "=" << result);
            return result;
        }
    break;
 }

 throw UTF8_Conversion() << "Invalid character: " << std::hex << actual;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                                       *
 *       class FromWstring:                                                              *
 *                                                                                       *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/// Convert a Unicode string to UTF8
FromWstring::FromWstring(const WChar * p_str):
    myLength(CalculateLength(p_str)),
    myStr(new char[myLength+1])
{
 SYS_DEBUG_MEMBER(DM_UTF8);

 char * ch;
 for (ch = myStr.get(); *p_str; ) {
    GetChar(*p_str++, ch);
 }

 ASSERT(ch == myStr.get() + myLength) << ": UTF8 string length calculation problem";

 *ch = '\0';
}

/// Convert one Unicode character to UTF8
void FromWstring::GetChar(WChar p_char, char *& p_result)
{
 SYS_DEBUG_STATIC(DM_UTF8);

 if ((p_char & ~ONE_BYTE_MASK) == 0) {
    *p_result++ = (char)(p_char & ONE_BYTE_MASK);
    return;
 } else if ((p_char & ~TWO_BYTE_MASK) == 0) {
    *p_result++ = (char)(TWO_BYTE_ENCODE     |  (p_char >> 6));
    *p_result++ = (char)(CONTINUATION_ENCODE |  (p_char & CONTINUATION_MASK));
    return;
 } else if ((p_char & ~THREE_BYTE_MASK) == 0) {
    *p_result++ = (char)(THREE_BYTE_ENCODE   |  (p_char >> 12));
    *p_result++ = (char)(CONTINUATION_ENCODE | ((p_char >> 6) & CONTINUATION_MASK));
    *p_result++ = (char)(CONTINUATION_ENCODE |  (p_char & CONTINUATION_MASK));
    return;
 } else if ((p_char & ~FOUR_BYTE_MASK) == 0) {
    *p_result++ = (char)(FOUR_BYTE_ENCODE    |  (p_char >> 18));
    *p_result++ = (char)(CONTINUATION_ENCODE | ((p_char >> 12) & CONTINUATION_MASK));
    *p_result++ = (char)(CONTINUATION_ENCODE | ((p_char >> 6) & CONTINUATION_MASK));
    *p_result++ = (char)(CONTINUATION_ENCODE |  (p_char & CONTINUATION_MASK));
    return;
 }

 throw UTF8_Conversion() << "Invalid wide character value: " << std::hex << (int)p_char;
}

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
