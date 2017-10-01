/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     My Generic C++ Library
 * Purpose:     
 * Author:      György Kövesdi (kgy@etiner.hu)
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

#include <iostream>

class ConfigData
{
 public:
    virtual ~ConfigData()
    {
    }

    virtual void ParseConfig(ConfigStore & conf) =0;
    virtual void SaveConfig(ConfigStore & conf) =0;

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

    void toStream(std::ostream & os) const
    {
        os << theConfig;
    }

    /// Generic function, getting any kind of config entry
    inline static const ConfigValue GetConfig(const std::string & key)
    {
        return Get().theConfig.GetConfig(key);
    }

    /// Gets a string entry from the config
    inline static const std::string & GetConfig(const std::string & key, const std::string & def_val)
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

    /// Set or update config entry
    inline static void SetConfig(const std::string & key, const std::string & value)
    {
        Get().theConfig.SetConfig(key, value);
    }

    void SaveConfig(void);

 protected:
    MainConfig(void);

    static MEM::shared_ptr<MainConfig> myself;

 private:
    SYS_DEFINE_CLASS_NAME("MainConfig");

    ConfigStore theConfig;

    static Threads::Mutex myMutex;

}; // class MainConfig

OSTREAM_OPERATOR_4(MainConfig);

#endif /* __SRC_CONFIG_MAINCONFIG_H_INCLUDED__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
