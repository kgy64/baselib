/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     My Generic C++ Library
 * Purpose:     
 * Author:      György Kövesdi (kgy@teledigit.eu)
 * Licence:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __SRC_CONFIG_MAINCONFIG_H_INCLUDED__
#define __SRC_CONFIG_MAINCONFIG_H_INCLUDED__

#include <Config/ConfigDriver.h>
#include <Threads/Threads.h>
#include <Threads/Mutex.h>
#include <Memory/Auton.h>
#include <Debug/Debug.h>

class ConfigData
{
 public:
    virtual ~ConfigData()
    {
    }

    virtual void ParseConfig(ConfigStore & conf) =0;

 private:
    SYS_DEFINE_CLASS_NAME("ConfigData");

}; // class ConfigData

class MainConfig
{
 public:
    VIRTUAL_IF_DEBUG inline ~MainConfig()
    {
    }

    static MainConfig & Get(void)
    {
        MainConfig * me = myself.get();
        if (!me) {
            Threads::Lock _l(myMutex);
            me = myself.get();
            if (!me) {
                me = new MainConfig();
                myself.reset(me);
            }
        }
        return *me;
    }

    /// Prints the whole config (for debug purpose)
    inline void List(void) const
    {
        theConfig.List();
    }

    /// Generic function, getting any kind of config entry
    inline static const ConfigValue GetConfig(const std::string & key)
    {
        return Get().theConfig.GetConfig(key);
    }

    /// Gets a string entry from the config
    /*! This is a specific case to prevent using temporary variables in a wrong way: the second argument
     *  is intentionally not 'const' to prevent converting 'const char *' or 'char *' to 'std::string' and
     *  returning its reference. So, if you got a "using temporary..." or "cannot convert const char *..."
     *  error message during build, probably it means your code is wrong. Use 'std::string' parameter for
     *  the default value. */
    inline static const std::string & GetConfig(const std::string & key, std::string & def_val)
    {
        return Get().theConfig.GetConfig(key, def_val);
    }

    inline static std::string FullPathOf(const std::string & rel_path)
    {
        return Get().theConfig.FullPathOf(rel_path);
    }

    inline static std::string GetRootDir(void)
    {
        return Get().theConfig.GetRootDir();
    }

    /// Gets any kind of config entry by type
    /*! \see    class \ref ConfigStore for the possible data types. */
    template <typename T>
    inline static T GetConfig(const std::string & key, T def_val)
    {
        return Get().theConfig.GetConfig(key, def_val);
    }

    inline static std::string GetPath(const std::string & key)
    {
        return Get().theConfig.GetPath(key);
    }

    inline static void AddConfig(const std::string & key, const std::string & value)
    {
        Get().theConfig.AddConfig(key, value);
    }

 protected:
    MainConfig(void);

    static MEM::shared_ptr<MainConfig> myself;

 private:
    SYS_DEFINE_CLASS_NAME("MainConfig");

    ConfigStore theConfig;

    static Threads::Mutex myMutex;

}; // class MainConfig

#endif /* __SRC_CONFIG_MAINCONFIG_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
