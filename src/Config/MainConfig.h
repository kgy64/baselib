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
        if (!myself) {
            Threads::Lock _l(myMutex);
            if (!myself) {
                myself.reset(new MainConfig());
            }
        }
        return *myself;
    }

    /// Prints the whole config (for debug purpose)
    void List(void) const
    {
        theConfig.List();
    }

    static const ConfigValue GetConfig(const std::string & key)
    {
        return Get().theConfig.GetConfig(key);
    }

    static const std::string & GetConfig(const std::string & key, const std::string & def_val);
    static int GetConfig(const std::string & key, int def_val);
    static float GetConfig(const std::string & key, float def_val);
    static double GetConfig(const std::string & key, double def_val);

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
