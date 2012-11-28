#include "DefaultImplement.h"

AUTON_INTERFACE(I_InternationalBase);

AUTON_IMPLEMENT(DefaultInternational, I_InternationalBase);

/// Default translation: does nothing
const char * DefaultInternational::Translate(const char * str) const
{
 return str;
}

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
