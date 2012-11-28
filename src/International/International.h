#ifndef __INTERNATIONAL_H__
#define __INTERNATIONAL_H__

#include <Memory/Auton.h>

/// Interface for Internationalization
class I_InternationalBase
{
 public:
    virtual ~I_InternationalBase()
    {
    }

    /// Translate the given string to local language
    /*! \note   If the stirng cannot be translated, the original string is returned.
     */
    virtual const char * Translate(const char * str) const =0;
};

/// Translate the given string to local language
/*! \see    I_InternationalBase::Translate() for details
 */
static inline const char * _I(const char * str)
{
    return Auton<I_InternationalBase>()->Translate(str);
}

#endif /* __INTERNATIONAL_H__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
