/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     My Generic C++ Library
 * Purpose:     
 * Author:      György Kövesdi (kgy@etiner.hu)
 * Licence:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "MainConfig.h"
#include <Exceptions/Exceptions.h>

AUTON_INTERFACE(ConfigData);

SYS_DEFINE_MODULE(DM_MAIN_CONFIG);

MEM::shared_ptr<MainConfig> MainConfig::myself;

Threads::Mutex MainConfig::myMutex;

MainConfig::MainConfig(void)
{
 SYS_DEBUG_MEMBER(DM_MAIN_CONFIG);

 Auton<ConfigData>()->ParseConfig(theConfig);
}

void MainConfig::SaveConfig(void)
{
 SYS_DEBUG_MEMBER(DM_MAIN_CONFIG);

 Auton<ConfigData>()->SaveConfig(theConfig);
}

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
