/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     The project Refleks
 * Purpose:     Measurements on speakers
 * Author:      Kövesdi György <kgy@etiner.hu>
 * License:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdlib.h>
#include <ctype.h>

#include <Debug/Debug.h>

#include "Decode.h"

SYS_DECLARE_MODULE(DM_FILE);

static std::string get_environment_var(const char *& s)
{
 SYS_DEBUG_FUNCTION(DM_FILE);

 std::string name;

 ++s;
 while (isalpha(*s) || *s == '_') {
    char c = *s++;
    name += c;
 }
 --s;

 const char * value = getenv(name.c_str());

 SYS_DEBUG(DL_INFO1, "Environment var '" << name << "' has value '" << value << "'");

 return value ? value : "";
}

std::string FILES::DecodeName(const char * name)
{
 SYS_DEBUG_FUNCTION(DM_FILE);

 std::string result;
 const char * chunk = nullptr;
 unsigned int size = 0;

 for (const char * p = name; *p; ++p) {
    switch (*p) {
        case '$':
            if (size) {
                result += std::string(chunk, size);
            }
            size = 0;
            chunk = nullptr;

            result += get_environment_var(p);
        break;

        case '~':
            if (size) {
                result += std::string(chunk, size);
            }
            size = 0;
            chunk = nullptr;
            result += getenv("HOME");
        break;

        default:
            if (!chunk) {
                chunk = p;
            }
            ++size;
        break;
    }
 }

 if (size) {
    result += std::string(chunk, size);
 }

 SYS_DEBUG(DL_INFO1, "Decoded name: '" << result << "'");

 return result;
}

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
