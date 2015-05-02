#ifndef __SRC_CONFIG_CONFIGDRIVER_H_INCLUDED__
#define __SRC_CONFIG_CONFIGDRIVER_H_INCLUDED__

#include <File/FileMapTyped.h>
#include <Memory/Memory.h>
#include <Debug/Debug.h>

#include <map>
#include <string>

class ConfExpression;
class AssignmentSet;
class ConfAssign;
class ConfDriver;
class ConfigLevel;

typedef MEM::shared_ptr<ConfExpression> ConfigValue;

#include <Config/ConfigHandler.hpp> // Generated by bison

SYS_DECLARE_MODULE(DM_CONFIG);

class ConfigStore
{
 public:
    ConfigStore(void)
    {
    }

    VIRTUAL_IF_DEBUG ~ConfigStore()
    {
    }

    inline void SetConfig(AssignmentSet * assigns)
    {
        theConfig = MEM::shared_ptr<AssignmentSet>(assigns);
    }

    void List(void) const;
    const ConfigValue GetConfig(const std::string & key) const;
    const std::string & GetConfig(const std::string & key, const std::string & def_val);
    int GetConfig(const std::string & key, int def_val);
    float GetConfig(const std::string & key, float def_val);
    double GetConfig(const std::string & key, double def_val);
    std::string GetPath(const std::string & key);
    const std::string & GetRootDir(void);
    std::string FullPathOf(const std::string & rel_path);
    void AddConfig(const std::string & key, const std::string & value);

    inline const std::string & GetDefaultRootDirecories(void) const
    {
        return default_root_directory_list;
    }

    inline void SetDefaultRootDirecories(const char * dirs)
    {
        default_root_directory_list = dirs;
    }

    inline void SetDefaultRootDirecories(const std::string & dirs)
    {
        default_root_directory_list = dirs;
    }

 private:
    SYS_DEFINE_CLASS_NAME("ConfigStore");

 protected:
    MEM::shared_ptr<AssignmentSet> theConfig;

    std::string root_directory;

    std::string default_root_directory_list;

}; // class ConfigStore

class ConfDriver
{
 public:
    ConfDriver(FILES::FileMap_char & file_2_parse, ConfigStore & store);
    ConfDriver(const char * data, int length, ConfigStore & store);

    int parse();
    void error(const yy::location & loc, const std::string & message);
    void AddError(void);
    int yylex(yy::ConfParser::semantic_type & yylval, yy::ConfParser::location_type & yylloc);
    int lexical_analyzer(yy::ConfParser::semantic_type & yylval);

    void Declare(AssignmentSet * assigns)
    {
        configStore.SetConfig(assigns);
    }

 private:
    SYS_DEFINE_CLASS_NAME("ConfDriver");

    int ChrGet(void);
    void UnGet(void);

    FILES::FileMap_char * file;

    const char * myData;
    int myLength;
    int myPosition;

    int lineNo;
    int column;

    ConfigStore & configStore;

}; // class ConfDriver

typedef MEM::shared_ptr<ConfDriver> ConfDriverPtr;

static std::ostream & operator<<(std::ostream & os, const ConfExpression & ex);

class ConfExpression
{
 friend std::ostream & operator<<(std::ostream & os, const ConfExpression & ex);

 public:
    ConfExpression(const std::string & value);
    VIRTUAL_IF_DEBUG ~ConfExpression();

    const std::string & GetString(void) const { return myValue; }

    const int * ToInt(void) const { return iValue.Value_p; }

    const float * ToFloat(void) const { return fValue.Value_p; }

    const double * ToDouble(void) const { return dValue.Value_p; }

    void reference(void)
    {
        SYS_DEBUG_MEMBER(DM_CONFIG);
        ++reference_counter;
        SYS_DEBUG(DL_INFO2, "Refernced: " << reference_counter);
    }

    void unreference(void)
    {
        SYS_DEBUG_MEMBER(DM_CONFIG);
        if (--reference_counter) {
            SYS_DEBUG(DL_INFO2, "Refernced: " << reference_counter);
            return;
        }
        SYS_DEBUG(DL_INFO2, "To be deleted...");
        delete this;
    }

    void Sign(void)
    {
        *iValue.Value_p = - *iValue.Value_p;
        *fValue.Value_p = - *fValue.Value_p;
        *dValue.Value_p = - *dValue.Value_p;
    }

 private:
    SYS_DEFINE_CLASS_NAME("ConfExpression");

    std::string myValue;

    template <typename T>
    struct Value_t
    {
        void Validate(bool ok)
        {
            Value_p = ok ? &Value : NULL;
        }

        T Value;

        T * Value_p;
    };

    Value_t<int> iValue;

    Value_t<float> fValue;

    Value_t<double> dValue;

    int reference_counter;

}; // class ConfExpression

static inline std::ostream & operator<<(std::ostream & os, const ConfExpression & ex)
{
 os << ex.myValue;
 return os;
}

static std::ostream & operator<<(std::ostream & os, const ConfAssign & as);

class ConfAssign
{
 friend std::ostream & operator<<(std::ostream & os, const ConfAssign & as);

 public:
    ConfAssign(ConfigValue & name, ConfigValue & value):
        reference_counter(1),
        myName(name),
        myValue(value)
    {
        SYS_DEBUG_MEMBER(DM_CONFIG);
    }

    VIRTUAL_IF_DEBUG ~ConfAssign()
    {
        SYS_DEBUG_MEMBER(DM_CONFIG);
    }

    ConfigValue & GetName(void) { return myName; }

    ConfigValue & GetValue(void) { return myValue; }

    void reference(void)
    {
        SYS_DEBUG_MEMBER(DM_CONFIG);
        ++reference_counter;
        SYS_DEBUG(DL_INFO2, "Refernced: " << reference_counter);
    }

    void unreference(void)
    {
        SYS_DEBUG_MEMBER(DM_CONFIG);
        if (--reference_counter) {
            SYS_DEBUG(DL_INFO2, "Refernced: " << reference_counter);
            return;
        }
        SYS_DEBUG(DL_INFO2, "To be deleted...");
        delete this;
    }

 private:
    SYS_DEFINE_CLASS_NAME("ConfAssign");

    int reference_counter;

    ConfigValue myName;
    ConfigValue myValue;

}; // class ConfAssign

static inline std::ostream & operator<<(std::ostream & os, const ConfAssign & as)
{
 os << "Assign(" << as.myName << "=" << as.myValue << ")";
 return os;
}

static std::ostream & operator<<(std::ostream & os, const AssignmentSet & body);

typedef MEM::shared_ptr<ConfigLevel> ConfPtr;

class AssignmentSet
{
 friend std::ostream & operator<<(std::ostream & os, const AssignmentSet & body);

 public:
    inline AssignmentSet(void):
        reference_counter(1)
    {
        SYS_DEBUG_MEMBER(DM_CONFIG);
    }

    VIRTUAL_IF_DEBUG inline ~AssignmentSet()
    {
        SYS_DEBUG_MEMBER(DM_CONFIG);
        SYS_DEBUG(DL_INFO2, "Size was " << assigns.size() << "/" << subConfigs.size());
    }

    const ConfigValue GetConfig(const std::string & key) const;
    AssignmentSet * Append(ConfAssign * assignment);
    AssignmentSet * Append(ConfigLevel * conf);
    AssignmentSet * Append(AssignmentSet * other);
    AssignmentSet * AddConfig(const std::string & key, const std::string & value);
    const std::string * GetValue(const std::string & name);
    const ConfPtr GetSubconfig(const std::string & name);
    void List(int level) const;

    inline void AppendValue(const std::string & key, ConfigValue value)
    {
        assigns[key] = value;
    }

    inline void AppendSubconfig(const std::string & key, ConfPtr conf)
    {
        subConfigs[key] = conf;
    }

    size_t noOfAssignments(void) const
    {
        return assigns.size();
    }

    size_t noOfSubconfigs(void) const
    {
        return subConfigs.size();
    }

    void reference(void)
    {
        SYS_DEBUG_MEMBER(DM_CONFIG);
        ++reference_counter;
        SYS_DEBUG(DL_INFO2, "Refernced: " << reference_counter);
    }

    void unreference(void)
    {
        SYS_DEBUG_MEMBER(DM_CONFIG);
        if (--reference_counter) {
            SYS_DEBUG(DL_INFO2, "Refernced: " << reference_counter);
            return;
        }
        SYS_DEBUG(DL_INFO2, "To be deleted...");
        delete this;
    }

 private:
    SYS_DEFINE_CLASS_NAME("AssignmentSet");

    typedef std::map<std::string, ConfigValue> AssignContainer;

    AssignContainer assigns;

    typedef std::map<std::string, ConfPtr> ConfigContainer;

    ConfigContainer subConfigs;

    int reference_counter;

}; // class AssignmentSet

static inline std::ostream & operator<<(std::ostream & os, const AssignmentSet & body)
{
 os << "AssignmentSet(" << body.assigns.size() << "/" << body.subConfigs.size() << ")";
 return os;
}

class ConfigLevel
{
 public:
    ConfigLevel(const std::string & name, AssignmentSet * body):
        levelName(name),
        assignments(body),
        reference_counter(1)
    {
    }

    ConfigLevel(ConfigValue & name, AssignmentSet * body):
        ConfigLevel(name->GetString(), body)
    {
    }

    VIRTUAL_IF_DEBUG ~ConfigLevel()
    {
        SYS_DEBUG_MEMBER(DM_CONFIG);
        SYS_DEBUG(DL_INFO2, "Name was: " << levelName);
    }

    const std::string & GetName(void) const
    {
        return levelName;
    }

    AssignmentSet & GetAssignments(void)
    {
        return *assignments;
    }

    const AssignmentSet & GetAssignments(void) const
    {
        return *assignments;
    }

    const ConfigValue GetConfig(const std::string & key) const;
    void List(int level) const;

    void reference(void)
    {
        SYS_DEBUG_MEMBER(DM_CONFIG);
        ++reference_counter;
        SYS_DEBUG(DL_INFO2, "Refernced: " << reference_counter);
    }

    void unreference(void)
    {
        SYS_DEBUG_MEMBER(DM_CONFIG);
        if (--reference_counter) {
            SYS_DEBUG(DL_INFO2, "Refernced: " << reference_counter);
            return;
        }
        SYS_DEBUG(DL_INFO2, "To be deleted...");
        delete this;    // Ugly (TODO: do it in a better way)
    }

 private:
    SYS_DEFINE_CLASS_NAME("ConfigLevel");

    std::string levelName;
    MEM::shared_ptr<AssignmentSet> assignments;

    int reference_counter;

}; // class ConfigLevel

static inline std::ostream & operator<<(std::ostream & os, const ConfigLevel & conf)
{
 os << conf.GetName() << "(" << conf.GetAssignments().noOfAssignments() << "/"
                             << conf.GetAssignments().noOfSubconfigs() << ")";
 return os;
}

#endif /* __SRC_CONFIG_CONFIGDRIVER_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
