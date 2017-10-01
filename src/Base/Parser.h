/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     My Generic C++ Library
 * Purpose:     
 * Author:      György Kövesdi (kgy@etiner.hu)
 * Licence:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __SRC_BASE_PARSER_H_INCLUDED__
#define __SRC_BASE_PARSER_H_INCLUDED__

#include <vector>
#include <Memory/Memory.h>

#include <Debug/Debug.h>

SYS_DECLARE_MODULE(DM_PARSER);

namespace Parser
{
    long StrtolSafe(const char * p_str, int base=10);
    long long StrtollSafe(const char * p_str, int base=10);
    double StrtodSafe(const char * p_str);

    class Tokenizer
    {
     public:
        static const char default_delimiters[];

        Tokenizer(const char * p_text, const char * p_delimiters = default_delimiters);

        inline Tokenizer(const std::string & p_text, const char * p_delimiters = default_delimiters):
            Tokenizer(p_text.c_str(), p_delimiters)
        {
        }

        const char * operator[](int p_index) const;

        inline int size(void) const
        {
            return chunks.size();
        }

     protected:
        MEM::scoped_ptr<char> myText;

        std::vector<const char *> chunks;

     private:
        SYS_DEFINE_CLASS_NAME("Tokenizer");
    };
} // namespace Parser

#endif /* __SRC_BASE_PARSER_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
