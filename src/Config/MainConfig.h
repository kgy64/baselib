#ifndef __MAINCONFIG_H__
#define __MAINCONFIG_H__

#include <Config/ConfigDriver.h>
#include <Threads/Threads.h>
#include <Threads/Mutex.h>
#include <Debug/Debug.h>

class MainConfig
{
 public:
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

    static boost::shared_ptr<MainConfig> myself;

    static const char configName[];

 private:
    SYS_DEFINE_CLASS_NAME("MainConfig");

    ConfigStore theConfig;

    static Threads::Mutex myMutex;

}; // class MainConfig

#endif /* __MAINCONFIG_H__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
