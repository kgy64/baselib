/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     My Generic C++ Library
 * Purpose:     
 * Author:      György Kövesdi (kgy@etiner.hu)
 * Licence:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "Parser.h"

#include <Exceptions/Exceptions.h>

#include <string.h>
#include <stdlib.h>

#include <sstream>

SYS_DEFINE_MODULE(DM_PARSER);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                                       *
 *       Conversion Functions:                                                           *
 *                                                                                       *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

long Parser::StrtolSafe(const char * p_str, int base)
{
 ASSERT(p_str && *p_str, "NULL string conversion");
 char * ende;
 long result = strtol(p_str, &ende, base);
 ASSERT(!*ende, "strtol(): unconvertable string '" << ende << "'");
 return result;
}

long long Parser::StrtollSafe(const char * p_str, int base)
{
 ASSERT(p_str && *p_str, "NULL string conversion");
 char * ende;
 long long result = strtoll(p_str, &ende, base);
 ASSERT(!*ende, "strtoll(): unconvertable string '" << ende << "'");
 return result;
}

static inline double my_strtod(const char * s, const char ** endptr)
{
 std::istringstream _is(s);
 _is.imbue(std::locale::classic());
 double result;
 _is >> result;
 if (_is.fail()) {
    *endptr = s;
 } else {
    *endptr = s + _is.tellg();
 }
 return result;
}

/// Locale-independent conversion from ASCII to double
/*! Expects dot (.) as decimal point. */
double Parser::StrtodSafe(const char * p_str)
{
 ASSERT(p_str && *p_str, "NULL string conversion");
 const char * ende;
 double result = my_strtod(p_str, &ende);
 ASSERT(!*ende, "strtod(): unconvertable string '" << ende << "'");
 return result;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

using namespace Parser;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                                       *
 *       class Tokenizer:                                                                *
 *                                                                                       *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

const char Tokenizer::default_delimiters[] = " \t\r\n";

Tokenizer::Tokenizer(const char * p_text, const char * p_delimiters)
{
 SYS_DEBUG_MEMBER(DM_PARSER);

 myText.reset(strdup(p_text));

 SYS_DEBUG(DL_VERBOSE, "To be tokenized: '" << myText.get() << "'");

 bool inserted = false;
 for (const char * p = myText.get(); *p; ++p) {
    if (strchr(p_delimiters, *p)) {
        *const_cast<char *>(p) = 0;
        inserted = false;
    } else {
        if (!inserted) {
            chunks.push_back(p);
            inserted = true;
        }
    }
 }

 SYS_DEBUG(DL_VERBOSE, " - There are " << chunks.size() << " chunks:");
 for (size_t i=0; i<chunks.size(); ++i) {
    SYS_DEBUG(DL_VERBOSE, " - [" << i << "]: '" << (*this)[i] << "'");
 }
}

const char * Tokenizer::operator[](int p_index) const
{
 if (p_index < 0 || p_index >= (int)chunks.size()) {
    return NULL;
 }
 return chunks[p_index];
}

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
