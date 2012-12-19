#include "MainConfig.h"
#include <Exceptions/ICExceptions.h>

SYS_DEFINE_MODULE(DM_MAIN);

MainConfig * MainConfig::myself = NULL;

MainConfig::MainConfig(void)
{
 SYS_DEBUG_MEMBER(DM_MAIN);

 FileMap_char configFile(configName);

 SYS_DEBUG(DL_INFO1, "Main config file: size=" << configFile.GetSize());

 ConfDriver parser(configFile, theConfig);

 if (parser.parse() != 0) {
    SYS_DEBUG(DL_INFO1, "Error parsing config file " << configName << ", some settings may be incorrect.");
 }

 SYS_DEBUG(DL_INFO1, "Config file " << configName << " parsed.");
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
