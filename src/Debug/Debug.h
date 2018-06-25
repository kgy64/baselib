/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        Debug.h
 * Purpose:     Class for debugging
 * Author:      Kövesdi György  (kgy@etiner.hu)
 * Modified by: KGy 2013-05-31: The stdc++-based version
 *              KGy 2013-10-20: Customizable output stream
 * Created:     2006-03-28
 * Licence:     GPL
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __SRC_DEBUG_DEBUG_H_INCLUDED__
#define __SRC_DEBUG_DEBUG_H_INCLUDED__

#if SYS_DEBUG_ON

#include "debug-internal.h"

#else   //  SYS_DEBUG_ON

#include <iostream>

#define DEBUG_IS_ON     false
#define DEBUG_IS_OFF    true

// Debug is switched off.

#define DEBUG_OUT(msg)  std::cerr << msg << std::endl

// Empty definitions for debug macros:

#define SYS_DEBUG_OFF                               { }
#define SYS_DEBUG(level, msg)                       { }
#define SYS_DEBUG_FUNCTION(name)                    { }
#define SYS_DEBUG_STATIC(name)                      { }
#define SYS_DEBUG_MEMBER_NAME(name, module_name)    { }
#define SYS_DEBUG_MEMBER(module_name)               { }
#define SYS_DEBUGLEVEL(level)                       { }
#define SYS_DEBUGLEVEL_MODULE(name, level)          { }
#define SYS_DEBUG_MODULE(name, onoff)               { }
#define SYS_DEBUG_MODULE_ON(name)                   { }
#define SYS_DEBUG_MODULE_OFF(name)                  { }
#define SYS_DEFINE_CLASS_NAME(name)
#define SYS_DEBUG_ALL_MODULES(onoff)
#define SYS_DECLARE_MODULE(name)
#define SYS_DEFINE_MODULE(name)
#define SYS_DEBUG_ALL_MODULES_ON
#define SYS_DEBUG_ALL_MODULES_OFF
#define VIRTUAL_IF_DEBUG

#endif  // !SYS_DEBUG_ON

#define YESNO(value)    ((value) ? "yes" : "no")

#define OSTREAM_OPERATOR_4(class_name) \
static inline std::ostream & operator<<(std::ostream & os, const class_name & d) \
{ \
 d.toStream(os); \
 return os; \
}

#endif // __SRC_DEBUG_DEBUG_H_INCLUDED__

/* ------------------------- End - of - file --------------------------- */
