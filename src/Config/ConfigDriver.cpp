#include "ConfigDriver.h"

#include <Base/Parser.h>

#include <stdlib.h>
#include <ctype.h>
#include <iostream>

SYS_DEFINE_MODULE(DM_CONFIG);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                                       *
 *     class ConfigStore:                                                                *
 *                                                                                       *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

const ConfigValue ConfigStore::GetConfig(const std::string & key) const
{
 SYS_DEBUG_MEMBER(DM_CONFIG);
 SYS_DEBUG(DL_INFO1, "Finding key '" << key << "' from root...");
 if (!theConfig) {
    SYS_DEBUG(DL_INFO1, "No root config, nothing found.");
    return ConfigValue(); // NULL
 }
 return theConfig->GetConfig(key);
}

const std::string & ConfigStore::GetConfig(const std::string & key, const std::string & def_val)
{
 ConfigValue val = GetConfig(key);
 if (!val)
    return def_val;
 return val->GetString();
}

int ConfigStore::GetConfig(const std::string & key, int def_val)
{
 ConfigValue val = GetConfig(key);
 if (!val)
    return def_val;
 const int * ip = val->ToInt();
 if (!ip) {
    // This is really an error: requested something which has wrong type:
    throw EX::Error("Value cannot be converted to int");
 }
 return *ip;
}

float ConfigStore::GetConfig(const std::string & key, float def_val)
{
 ConfigValue val = GetConfig(key);
 if (!val)
    return def_val;
 const float * fp = val->ToFloat();
 if (!fp) {
    // This is really an error: requested something which has wrong type:
    throw EX::Error("Value cannot be converted to float");
 }
 return *fp;
}

double ConfigStore::GetConfig(const std::string & key, double def_val)
{
 ConfigValue val = GetConfig(key);
 if (!val)
    return def_val;
 const double * dp = val->ToDouble();
 if (!dp) {
    // This is really an error: requested something which has wrong type:
    throw EX::Error("Value cannot be converted to double float");
 }
 return *dp;
}

void ConfigStore::AddConfig(const std::string & key, const std::string & value)
{
 SYS_DEBUG_MEMBER(DM_CONFIG);

 if (!theConfig) {  // Hopefully it runs on one thread yet
    theConfig.reset(new AssignmentSet());   // Add an empty set
 }

 theConfig->AddConfig(key, value);
}

std::string ConfigStore::GetPath(const std::string & key)
{
 const ConfigValue value = GetConfig(key);
 ASSERT(value, "path entry missing from config: '" << key << "'");
 return FullPathOf(value->GetString());
}

const std::string & ConfigStore::GetRootDir(void)
{
 SYS_DEBUG_MEMBER(DM_CONFIG);

 if (root_directory.empty()) {
    SYS_DEBUG(DL_INFO1, "Calculating root directory...");
    root_directory = ".";   // default value
    const ConfigValue chain = GetConfig("SuperConfig");
    if (chain) {
        std::string root_dirs = GetConfig("RootDirectories", GetDefaultRootDirecories());
        Parser::Tokenizer tok(root_dirs);
        for (int i = 0; i < tok.size(); ++i) {
            std::string super_config_path = std::string(tok[i]) + "/" + chain->GetString();
            SYS_DEBUG(DL_INFO1, "Android Config: Trying to read Super Config from '" << super_config_path << "'");
            try {
                FILES::FileMap_char configFile(super_config_path.c_str());
                SYS_DEBUG(DL_INFO1, "Super Config file: size=" << configFile.GetSize());
                ConfDriver super_parser(configFile, *this);
                if (super_parser.parse() != 0) {
                    SYS_DEBUG(DL_ERROR, "Error parsing Super Config file " << super_config_path << ", some settings may be incorrect.");
                }
                root_directory = tok[i];
                SYS_DEBUG(DL_INFO1, "Super Config file " << super_config_path << " parsed.");
                break;  // Only one Super Config file expected
            } catch (EX::Assert & ex) {
                SYS_DEBUG(DL_WARNING, "Warning: could not parse the Super Config file beracuse " << ex.what());
            }
        }
    }
    SYS_DEBUG(DL_INFO1, "Root directory is '" << root_directory << "'");
 }

 return root_directory;
}

std::string ConfigStore::FullPathOf(const std::string & rel_path)
{
 if (rel_path[0] == DIR_SEPARATOR) {
    return rel_path;
 }
 std::string result = GetRootDir();
 result += DIR_SEPARATOR_STR;
 result += rel_path;
 return result;
}

/// Prints the whole config (for debug purpose)
void ConfigStore::List(void) const
{
 if (theConfig)
    theConfig->List(0);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                                       *
 *     class ConfigDriver:                                                               *
 *                                                                                       *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/// Config Driver from Config File
ConfDriver::ConfDriver(FILES::FileMap_char & p_file, ConfigStore & store):
    file(&p_file),
    myData(0),
    myLength(0),
    myPosition(0),
    lineNo(1),
    column(1),
    configStore(store)
{
}

/// Config Driver from Config Data in memory
ConfDriver::ConfDriver(const char * data, int length, ConfigStore & store):
    file(0),
    myData(data),
    myLength(length),
    myPosition(0),
    lineNo(1),
    column(1),
    configStore(store)
{
}

void ConfDriver::AddError(void)
{
 SYS_DEBUG_MEMBER(DM_CONFIG);
 SYS_DEBUG(DL_INFO1, "Error somewhere before line " << lineNo << " column " << column);
}

void ConfDriver::error(const yy::location & loc, const std::string & message)
{
 SYS_DEBUG_STATIC(DM_CONFIG);
 SYS_DEBUG(DL_INFO1, "Error at " << loc.begin << "-" << loc.end << ": " << message);
}

int ConfDriver::ChrGet(void)
{
 int result = 0;
 if (file) {
    result = file->ChrGet();
 } else {
    if (myPosition >= myLength) {
        result = -1;
    } else {
        result = myData[myPosition++];
    }
 }
 return result;
}

void ConfDriver::UnGet(void)
{
 if (file) {
    file->UnGet();
 } else {
    if (myPosition > 0 && myPosition < myLength) {
        --myPosition;
    }
 }
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
     while (isspace(ch = ChrGet())) {
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

     UnGet();

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

 int ch = ChrGet();

 if (ch < 0) {
    SYS_DEBUG(DL_INFO1, "End-of-file detected at line " << lineNo);
    return ch;
 }

 std::string st;

 if (isdigit(ch)) {
    for (;;) {
        st.append(1, (char)ch);
        ch = ChrGet();
        if (!isdigit(ch)) {
            UnGet();
            break;
        }
        ++column;
    }
    SYS_DEBUG(DL_INFO1, "NAME(" << st << ") is returned as a number");
    yylval.name = new ConfigValue(new ConfExpression(st));
    return yy::ConfParser::token::NAME;
 } else switch (ch) {
    case '"':
        for (;;) {
            ch = ChrGet();
            if (ch <= 0) {
                SYS_DEBUG(DL_ERROR, "String is not terminated [1] at line " << lineNo);
                return -1;
            }
            ++column;
            if (ch == '"')
                break;
            switch (ch) {
                case '\\':
                    ch = ChrGet();
                    if (ch <= 0) {
                        SYS_DEBUG(DL_ERROR, "String is not terminated [2] at line " << lineNo);
                        return -1;
                    }
                    switch (ch) {
                        case '\r':
                        case '\n':
                            ++lineNo;
                            column = 1;
                            // Ignore this EOL:
                            continue;
                        break;
                        case 'n':
                            ch = '\n';
                        break;
                        case 'r':
                            ch = '\r';
                        break;
                        case 't':
                            ch = '\t';
                        break;
                        case 'b':
                            ch = '\b';
                        break;
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
        SYS_DEBUG(DL_INFO1, "NAME(" << st << ") is returned as a quoted string");
        yylval.name = new ConfigValue(new ConfExpression(st));
        return yy::ConfParser::token::NAME;
    break;

    case '#':
        // Skip the whole line until EOL:
        SYS_DEBUG(DL_INFO1, "Line " << lineNo << " is a comment");
        while ((ch = ChrGet()) != '\r' && ch != '\n' && ch >= 0)
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
         SYS_DEBUG(DL_INFO1, "Returning single character: " << ch << ", ch: " << (char)ch);
         return ch;
    break;
 }

 for (;;) {
    if (ch == '\\') {
        ch = ChrGet();
        ++column;
    }
    st.append(1, (char)ch);
    ch = ChrGet();
    if (!isalnum(ch) && ch != '_' && ch != '\\') {
        UnGet();
        break;
    }
    ++column;
 }

 SYS_DEBUG(DL_INFO1, "NAME(" << st << ") is returned as a string");
 yylval.name = new ConfigValue(new ConfExpression(st));
 return yy::ConfParser::token::NAME;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                                       *
 *     class ConfExpression:                                                             *
 *                                                                                       *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

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

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                                       *
 *     class AssignmentSet:                                                              *
 *                                                                                       *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

AssignmentSet * AssignmentSet::Append(ConfAssign * assignment)
{
 SYS_DEBUG_MEMBER(DM_CONFIG);
 SYS_DEBUG(DL_INFO1, "Appended " << *assignment);
 AppendValue(assignment->GetName()->GetString(), assignment->GetValue());
 delete assignment; // due to bison's stupidity :-)
 return this;
}

AssignmentSet * AssignmentSet::Append(ConfigLevel * conf)
{
 SYS_DEBUG_MEMBER(DM_CONFIG);
 SYS_DEBUG(DL_INFO1, "Appended " << *conf);
 AppendSubconfig(conf->GetName(), ConfPtr(conf));
 return this;
}

AssignmentSet * AssignmentSet::Append(AssignmentSet * other)
{
 SYS_DEBUG_MEMBER(DM_CONFIG);
 SYS_DEBUG(DL_INFO1, "Appended " << *other);
 for (AssignContainer::iterator i = other->assigns.begin(); i != other->assigns.end(); ++i) {
    AppendValue(i->first, i->second);
 }
 for (ConfigContainer::iterator i = other->subConfigs.begin(); i != other->subConfigs.end(); ++i) {
    AppendSubconfig(i->first, i->second);
 }
 delete other; // Due to bison's stupidity :-)
 return this;
}

AssignmentSet * AssignmentSet::AddConfig(const std::string & key, const std::string & value)
{
 SYS_DEBUG_MEMBER(DM_CONFIG);

 std::string::size_type pos = key.find('/');
 if (pos == std::string::npos) {
    AppendValue(key, ConfigValue(new ConfExpression(value)));
 } else {
    std::string path = key.substr(0, pos);
    std::string subkey = key.substr(pos+1);
    ConfPtr subconfig = GetSubconfig(path);
    if (subconfig) {
        subconfig->GetAssignments().AddConfig(subkey, value);
    } else {
        Append(new ConfigLevel(path, (new AssignmentSet())->AddConfig(subkey, value)));
    }
 }

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
 static const char separators[] = "                                                ";
 for (AssignContainer::const_iterator i = assigns.begin(); i != assigns.end(); ++i) {
    int position = sizeof(separators) - level - 1;
    if (position >= 0) {
        DEBUG_OUT(separators+position << "\"" << i->first << "\"=\"" << *i->second << "\";");
    }
 }
 for (ConfigContainer::const_iterator i = subConfigs.begin(); i != subConfigs.end(); ++i) {
    int position = sizeof(separators) - level - 1;
    if (position >= 0) {
        DEBUG_OUT(separators+position << "/* " << i->first << ": */");
    }
    i->second->List(level);
 }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                                       *
 *     class ConfigLevel:                                                                *
 *                                                                                       *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

const ConfigValue ConfigLevel::GetConfig(const std::string & key) const
{
 SYS_DEBUG_MEMBER(DM_CONFIG);
 SYS_DEBUG(DL_INFO1, "Finding key '" << key << "' at " << levelName << " ...");

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
