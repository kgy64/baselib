/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     My Generic C++ Library
 * Purpose:     
 * Author:      György Kövesdi (kgy@teledigit.eu)
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
    myStr(new WChar[myLength+1]) // Note: it can be more than necessary
{
 SYS_DEBUG_MEMBER(DM_UTF8);

 size_t i;
 for (i = 0; (myStr[i] = GetChar(p_str)) != 0; ++i) ;

 SYS_DEBUG(DL_INFO1, "String length set to " << i << ", was " << myLength);
 myLength = i;
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
/*! \param  p_str   Pointer reference to the character to be converted. The pointer is advanced
                    past to the character accordingly.
    \retval WChar   The converted wide character is returned.
 */
WChar ToWstring::GetChar(const char *& p_str)
{
 SYS_DEBUG_STATIC(DM_UTF8);

 WChar actual = static_cast<WChar>(*reinterpret_cast<const unsigned char *>(p_str++));

 if (actual >= 0 && actual <= 0x7f) {
    // Simple ASCII character:
    SYS_DEBUG(DL_VERBOSE, " - Simple ASCII char: 0x" << std::hex << (unsigned int)actual);
    return actual;
 }

 switch (actual & 0xf0) {
    case 0xc0:
        if (actual < 0xc2) {
            // Wrong code:
            break;
        }
        // No break here! (this is also a 2-byte sequence)
    case 0xd0:
        {
            // 2-byte sequence (0x0080...0x07ff):
            int cont_1 = GetContinuationChar(p_str);
            int result = ((actual & 0x1f) << 6) | cont_1;
            SYS_DEBUG(DL_VERBOSE, " - 2-byte sequence: " << std::hex << (int)actual << "-" << cont_1 << "=" << result);
            return result;
        }
    break;
    case 0xe0:
        {
            // 3-byte sequence (0x0800...0xffff):
            int cont_1 = GetContinuationChar(p_str);
            int cont_2 = GetContinuationChar(p_str);
            int result = cont_2 | ((cont_1 | ((actual & 0x0f) << 6)) << 6);
            SYS_DEBUG(DL_VERBOSE, " - 3-byte sequence: " << std::hex << (int)actual << "-" << cont_1 << "-" << cont_2 << "=" << result);
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
            SYS_DEBUG(DL_VERBOSE, " - 4-byte sequence: " << std::hex << (int)actual << "-" << cont_1 << "-" << cont_2 << "-" << cont_3 << "=" << result);
            return result;
        }
    break;
 }

 ASSERT_T(UTF8_Conversion, false, "Invalid UTF8 character: " << (int)actual);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                                       *
 *       class FromWstring:                                                              *
 *                                                                                       *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/// Convert a Unicode string to UTF8
FromWstring::FromWstring(const WChar * p_str)
{
 SYS_DEBUG_MEMBER(DM_UTF8);

 myLength = CalculateLength(p_str);
 myStr.reset(new char[myLength+1]);

 char * ch = myStr.get();

 while (*p_str) {
    GetChar(*p_str++, ch);
 }

 ASSERT_DBG(ch == myStr.get() + myLength, "UTF8 string length calculation problem");

 *ch = '\0';
}

/// Convert one Unicode character to UTF8
/*! \param  p_char      The wide character to be converted
    \param  p_result    Pointer refernce of the result. It is advanced pasto to the currently converted
                        character, but nothing is stored beyond it.
 */
void FromWstring::GetChar(WChar p_char, char *& p_result)
{
 SYS_DEBUG_STATIC(DM_UTF8);

 SYS_DEBUG(DL_INFO2, "Converting 0x" << std::hex << (int)p_char);

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

 ASSERT_T(UTF8_Conversion, false, "Invalid wide character value: " << (int)p_char);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                                       *
 *       Functions:                                                                      *
 *                                                                                       *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/// Seek in a UTF8 string
/*! \param  p_str       The input string to seek
    \param  p_offset    Number of characters to seek. It means number of UTF8 Characters, not bytes, of course.<br>
                        If it is less than 1, nothing happens. If greater than the string length, then a pointer
                        to the closing null character is returned.
    \retval result      Pointer to the requested character is returned. Note that it points into the input
                        string, calculating the UTF8 character boundaries.
 */
const char * UTF8::Seek(const char * p_str, int p_offset)
{
 SYS_DEBUG_STATIC(DM_UTF8);

 do {
    if (p_offset <= 0) {
        return p_str;
    }

    --p_offset;

    unsigned int actual = static_cast<unsigned int>(*reinterpret_cast<const unsigned char *>(p_str));

    if (actual == 0U) {
        // End of string:
        return p_str;
    }

    ++p_str;

    if (actual <= 0x7f) {
        // Simple ASCII character:
        continue;
    }

    switch (actual & 0xf0) {
        case 0xc0:
            if (actual < 0xc2) {
                // Wrong code:
                ASSERT_T(UTF8_Conversion, false, "Invalid UTF8 character: " << (int)actual);
            }
            // No break here! (this is also a 2-byte sequence)
        case 0xd0:
            // 2-byte sequence (0x0080...0x07ff):
            p_str += 1;
        break;
        case 0xe0:
            // 3-byte sequence (0x0800...0xffff):
            p_str += 2;
        break;
        case 0xf0:
            // 4-byte sequence (>=0x10000):
            p_str += 3;
        break;
        default:
            ASSERT_T(UTF8_Conversion, false, "Invalid UTF8 character: " << (int)actual);
        break;
    }
 } while (true);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                                       *
 *       Operators:                                                                      *
 *                                                                                       *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

std::ostream & operator<<(std::ostream & os, const UTF8::WChar * st)
{
 UTF8::FromWstring utf8(st);
 return os << utf8.ToChar();
}

std::ostream & operator<<(std::ostream & os, UTF8::WChar ch)
{
 switch (ch) {
    case L'\0':
        return os << "\\0";
    break;
    case L'\r':
        return os << "\\r";
    break;
    case L'\n':
        return os << "\\n";
    break;
    case L'\t':
        return os << "\\t";
    break;
    case L'\'':
        return os << "\\'";
    break;
    case L'"':
        return os << "\\\"";
    break;
 }

 char result[8];
 char * p = result;
 UTF8::FromWstring::GetChar(ch, p);
 *p = '\0';

 return os << result;
}

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
