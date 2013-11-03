/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     
 * Purpose:     
 * Author:      
 * Licence:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "MainConfig.h"
#include <Exceptions/Exceptions.h>

AUTON_INTERFACE(ConfigData);

SYS_DEFINE_MODULE(DM_MAIN_CONFIG);

boost::shared_ptr<MainConfig> MainConfig::myself;

Threads::Mutex MainConfig::myMutex;

MainConfig::MainConfig(void)
{
 SYS_DEBUG_MEMBER(DM_MAIN_CONFIG);

 Auton<ConfigData> conf;
 conf->ParseConfig(theConfig);
}

const std::string & MainConfig::GetConfig(const std::string & key, const std::string & def_val)
{
 ConfigValue val = Get().theConfig.GetConfig(key);
 if (!val)
    return def_val;
 return val->GetString();
}

int MainConfig::GetConfig(const std::string & key, int def_val)
{
 ConfigValue val = Get().theConfig.GetConfig(key);
 if (!val)
    return def_val;
 const int * ip = val->ToInt();
 if (!ip) {
    // This is really an error: requested something which has wrong type:
    throw EX::Error("Value cannot be converted to int");
 }
 return *ip;
}

float MainConfig::GetConfig(const std::string & key, float def_val)
{
 ConfigValue val = Get().theConfig.GetConfig(key);
 if (!val)
    return def_val;
 const float * fp = val->ToFloat();
 if (!fp) {
    // This is really an error: requested something which has wrong type:
    throw EX::Error("Value cannot be converted to float");
 }
 return *fp;
}

double MainConfig::GetConfig(const std::string & key, double def_val)
{
 ConfigValue val = Get().theConfig.GetConfig(key);
 if (!val)
    return def_val;
 const double * dp = val->ToDouble();
 if (!dp) {
    // This is really an error: requested something which has wrong type:
    throw EX::Error("Value cannot be converted to double float");
 }
 return *dp;
}

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
