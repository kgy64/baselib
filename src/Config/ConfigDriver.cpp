#include "ConfigDriver.h"

#include <stdlib.h>
#include <ctype.h>
#include <iostream>

const ConfigValue ConfigStore::GetConfig(const std::string & key) const
{
 SYS_DEBUG_MEMBER(DM_CONFIG);
 SYS_DEBUG(DL_CONFIG, "Finding key '" << key << "' from root...");
 if (!theConfig) {
    SYS_DEBUG(DL_CONFIG, "No root config, nothing found.");
    return ConfigValue(); // NULL
 }
 return theConfig->GetConfig(key);
}

/// Prints the whole config (for debug purpose)
void ConfigStore::List(void) const
{
 if (theConfig)
    theConfig->List(0);
}

ConfDriver::ConfDriver(FileMap_char & p_file, ConfigStore & store):
    file(p_file),
    lineNo(1),
    column(1),
    configStore(store)
{
}

void ConfDriver::AddError(void)
{
 SYS_DEBUG_MEMBER(DM_CONFIG);
 SYS_DEBUG(DL_CONFIG, "Error somewhere before line " << lineNo << " column " << column);
}

void ConfDriver::error(const yy::location & loc, const std::string & message)
{
 SYS_DEBUG_STATIC(DM_CONFIG);
 SYS_DEBUG(DL_CONFIG, "Error at " << loc.begin << "-" << loc.end << ": " << message);
}

int ConfDriver::parse(void)
{
 yy::ConfParser parser(*this);
 return parser.parse();
}

int ConfDriver::yylex(yy::ConfParser::semantic_type & yylval, yy::ConfParser::location_type & yylloc)
{
 SYS_DEBUG_MEMBER(DM_CONFIG);

 int ch;

 do {
     while (isspace(ch = file.ChrGet())) {
         switch (ch) {
             case '\r':
             case '\n':
                 ++lineNo;
                 column = 1;
             break;
             default:
                 ++column;
             break;
         }
     }

     file.UnGet();

     yylloc.begin.line = lineNo;
     yylloc.begin.column = column;

     ch = lexical_analyzer(yylval);

 } while (ch == yy::ConfParser::token::_AGAIN);

 yylloc.end.line = lineNo;
 yylloc.end.column = column;

 return ch;
}

int ConfDriver::lexical_analyzer(yy::ConfParser::semantic_type & yylval)
{
 SYS_DEBUG_MEMBER(DM_CONFIG);

 int ch = file.ChrGet();

 if (ch < 0) {
    SYS_DEBUG(DL_CONFIG, "End-of-file detected at line " << lineNo);
    return ch;
 }

 std::string st;

 if (isdigit(ch)) {
    for (;;) {
        st.append(1, (char)ch);
        ch = file.ChrGet();
        if (!isdigit(ch)) {
            file.UnGet();
            break;
        }
        ++column;
    }
    SYS_DEBUG(DL_CONFIG, "NAME(" << st << ") is returned as a number");
    yylval.name = new ConfigValue(new ConfExpression(st));
    return yy::ConfParser::token::NAME;
 } else switch (ch) {
    case '"':
        for (;;) {
            ch = file.ChrGet();
            if (ch <= 0) {
                // Error: string is not terminated
                return -1;
            }
            ++column;
            if (ch == '"')
                break;
            switch (ch) {
                case '\\':
                    ch = file.ChrGet();
                    if (ch <= 0) {
                        // Error: string is not terminated
                        return -1;
                    }
                    ++column;
                break;
                case '\r':
                case '\n':
                    ++lineNo;
                    column = 1;
                break;
            }
            st.append(1, (char)ch);
        }
        SYS_DEBUG(DL_CONFIG, "NAME(" << st << ") is returned as a quoted string");
        yylval.name = new ConfigValue(new ConfExpression(st));
        return yy::ConfParser::token::NAME;
    break;

    case '#':
        // Skip the whole line until EOL:
        while ((ch = file.ChrGet()) != '\r' && ch != '\n' && ch >= 0)
            ++column;
        switch (ch) {
            case '\r':
            case '\n':
                ++lineNo;
                column = 1;
            break;
        }
        return yy::ConfParser::token::_AGAIN;
    break;

    case '{':
    case '}':
    case '[':
    case ']':
    case '(':
    case ')':
    case '=':
    case '+':
    case '-':
    case '*':
    case '/':
    case '%':
    case ';':
    case ':':
         SYS_DEBUG(DL_CONFIG, "Returning single character: " << ch << ", ch: " << (char)ch);
         return ch;
    break;
 }

 for (;;) {
    if (ch == '\\') {
        ch = file.ChrGet();
        ++column;
    }
    st.append(1, (char)ch);
    ch = file.ChrGet();
    if (!isalnum(ch) && ch != '_' && ch != '\\') {
        file.UnGet();
        break;
    }
    ++column;
 }

 SYS_DEBUG(DL_CONFIG, "NAME(" << st << ") is returned as a string");
 yylval.name = new ConfigValue(new ConfExpression(st));
 return yy::ConfParser::token::NAME;
}

ConfExpression::ConfExpression(const std::string & value):
    myValue(value),
    reference_counter(1)
{
 SYS_DEBUG_MEMBER(DM_CONFIG);

 const char * str = myValue.c_str();
 char * endptr;

 iValue.Value = strtol(str, &endptr, 0);
 iValue.Validate(*str && !*endptr);

 dValue.Value = strtod(str, &endptr);
 dValue.Validate(*str && !*endptr);

 // The same as double, but converted:
 fValue.Value = (float)dValue.Value;
 fValue.Validate(dValue.Value_p);
}

ConfExpression::~ConfExpression()
{
 SYS_DEBUG_MEMBER(DM_CONFIG);
}

AssignmentSet * AssignmentSet::Append(ConfAssign * assignment)
{
 SYS_DEBUG_MEMBER(DM_CONFIG);
 SYS_DEBUG(DL_CONFIG, "Appended " << *assignment);
 assigns[assignment->GetName()->GetString()] = assignment->GetValue();
 delete assignment; // due to bison's stupidity :-)
 return this;
}

AssignmentSet * AssignmentSet::Append(ConfigLevel * conf)
{
 SYS_DEBUG_MEMBER(DM_CONFIG);
 SYS_DEBUG(DL_CONFIG, "Appended " << *conf);
 subConfigs[conf->GetName()] = Glib::RefPtr<ConfigLevel>(conf);
 return this;
}

AssignmentSet * AssignmentSet::Append(AssignmentSet * other)
{
 SYS_DEBUG_MEMBER(DM_CONFIG);
 SYS_DEBUG(DL_CONFIG, "Appended " << *other);
 for (AssignContainer::iterator i = other->assigns.begin(); i != other->assigns.end(); ++i) {
    assigns[i->first] = i->second;
 }
 for (ConfigContainer::iterator i = other->subConfigs.begin(); i != other->subConfigs.end(); ++i) {
    subConfigs[i->first] = i->second;
 }
 delete other; // Due to bison's stupidity :-)
 return this;
}

const ConfigValue AssignmentSet::GetConfig(const std::string & key) const
{
 SYS_DEBUG_MEMBER(DM_CONFIG);

 size_t slash = key.find_first_of('/');
 if (slash != std::string::npos) {
    // Split the string, find the next container level:
    ConfigContainer::const_iterator i = subConfigs.find(key.substr(0, slash));
    if (i == subConfigs.end())
        return ConfigValue(); // NULL
    return i->second->GetConfig(key.substr(slash+1));
 }

 AssignContainer::const_iterator i = assigns.find(key);
 if (i == assigns.end())
    return ConfigValue(); // NULL
 return i->second;
}

const ConfPtr AssignmentSet::GetSubconfig(const std::string & name)
{
 SYS_DEBUG_MEMBER(DM_CONFIG);

 ConfigContainer::iterator i = subConfigs.find(name);
 if (i == subConfigs.end())
    return ConfPtr();
 return i->second;
}

/// Prints the whole config (for debug purpose)
void AssignmentSet::List(int level) const
{
 for (AssignContainer::const_iterator i = assigns.begin(); i != assigns.end(); ++i) {
    for (int j = 0; j < level; ++j)
        std::cout << "  ";
    std::cout << "\"" << i->first << "\"=\"" << i->second << "\";" << std::endl;
 }
 for (ConfigContainer::const_iterator i = subConfigs.begin(); i != subConfigs.end(); ++i) {
    for (int j = 0; j < level; ++j)
        std::cout << "  ";
    std::cout << "/* " << i->first << ": */" << std::endl;
    i->second->List(level);
 }
}

const ConfigValue ConfigLevel::GetConfig(const std::string & key) const
{
 SYS_DEBUG_MEMBER(DM_CONFIG);
 SYS_DEBUG(DL_CONFIG, "Finding key '" << key << "' at " << levelName << " ...");

 return GetAssignments().GetConfig(key);
}

/// Prints the whole config (for debug purpose)
void ConfigLevel::List(int level) const
{
 for (int j = 0; j < level; ++j)
     std::cout << "  ";
 std::cout << levelName << " {" << std::endl;
 if (assignments.get())
     assignments->List(level+1);
 for (int j = 0; j < level; ++j)
     std::cout << "  ";
 std::cout << "}" << std::endl;
}

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
