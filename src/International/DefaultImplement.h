#ifndef __DEFAULTIMPLEMENT_H__
#define __DEFAULTIMPLEMENT_H__

#include "International.h"

/// Default Internationalization
/*! This class does nothing, just returns the original string. It is necessary to be
    able to run the \ref _I() function without specifying any language.
 */
class DefaultInternational: public I_InternationalBase
{
 private:
    virtual const char * Translate(const char * str) const;
};

#endif /* __DEFAULTIMPLEMENT_H__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
