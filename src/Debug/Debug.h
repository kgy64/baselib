/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        Debug.h
 * Purpose:     Class for debugging
 * Author:      Kövesdi György  (kgy@teledigit.eu)
 * Modified by: KGy 2012-06-10: Use it in glib environment
 * Created:     2006-03-28
 * Licence:     GPL
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef _DEBUGPRINT_H_
#define _DEBUGPRINT_H_

#include <Memory/SPtr.h>
#include <iostream>
#include <sstream>
#include <map>
#include <glibmm/thread.h>
#include <string.h>
#include <stdint.h>

#include "debuglevels.h"

#ifdef __GNUC__
#define __FILE_LINE__   , __FILE__, __LINE__
#else
#define __FILE_LINE__
#endif

#if !defined(__GNUC__) && !defined(_VC2005)
// Define this macro as empty if it cannot be used:
#define __FUNCTION__
#define __FUNCTION_STRING__ "Unknown"
#else
#define __FUNCTION_WORKS__
#define __FUNCTION_STRING__ __FUNCTION__
#endif // __GNUC__ && _VC2005


#if SYS_DEBUG_ON
#define SYS_DEFINE_CLASS_NAME(__name) \
    virtual const char* className(void) const { return __name; }
#else
#define SYS_DEFINE_CLASS_NAME(__name)
#endif


class _DebugPrintLock: public Glib::Mutex::Lock
{
public:
    _DebugPrintLock(void):
        Glib::Mutex::Lock(debugMutex)
    {
    }

private:
    static Glib::Mutex debugMutex;
};

#define DEBUG_CRITICAL_SECTION  _DebugPrintLock __lock_

/// Debug message handler class
/*! This class is responsible for handling debug messages. The messages show
    the levels of nested function calls. If the functions are class members,
    the class name and 'this' pointer is also displayed. To do this, those
    classes (where the debug messages are printed from) must have a function
    to reach their class name (called className()).<br>
    This class can be fine-tuned by changing _DebugPrint::fill_1_begin,
    _DebugPrint::fill_1_end, _DebugPrint::fill_2 strngs.<br>
    <b>Usage:</b><br>
    Such a class can be constructed at the beginning of a function, as a
    local variable. The constructor and the destructor prints the 'entering...'
    and 'leaving...' messages. See the macros ::SYS_DEBUG_FUNCTION,
    ::SYS_DEBUG_MEMBER and ::SYS_DEBUG_STATIC to do this. <br>
    The _DebugPrint::debug_print() function can be used to print some
    messages from the actual function. See the macro ::SYS_DEBUG for
    details.<br>
    The messages are pinted in semi-graphic format, to be able to follow the
    program flow easily this way.
    \warning    It is not possible to use the debug macros from this class
                because it would lead to deadlock. */
class _DebugPrint
{
public:
    _DebugPrint(const char *name,
#ifdef __GNUC__
                     const char *fname, int lineno,
#endif
                     uint32_t module
                     );
    _DebugPrint(const void *thisptr, const char *classptr,
                      const char *name,
#ifdef __GNUC__
                     const char *fname, int lineno,
#endif
                     uint32_t module
                     );
    ~_DebugPrint();

    void draw_left(void);
    void endline(void);

    /*! This function just decides if the message must be printed or not. */
    static inline bool debug_print(uint32_t level) {
        return level & debuglevel;
    }

    /*! Returns true if the current module is switched on */
    inline bool debug_module(void) {
        return moduleIsOn;
    }

    /*! This function sets the current debuglevel (stored in the variable
        _DebugPrint::debuglevel). Returns the new level just set. */
    static inline uint32_t set_level(uint32_t level) { return debuglevel = level; }

    /*! This function sets the current modules switched on (stored in the variable
        _DebugPrint::debugmodules). Returns the new value just set. */
    static inline uint32_t set_modules(uint32_t modules) { return debugmodules = modules; }

    /*! This function returns the current debuglevel (stored in the variable _DebugPrint::debuglevel). */
    static inline uint32_t get_level(void) { return debuglevel; }

    /*! This function returns the current modules switched on (stored in the variable _DebugPrint::debugmodules). */
    static inline uint32_t get_modules(void) { return debugmodules; }

    _DebugPrint& operator<<(const std::ostringstream& p_string);

private:
    void entering(void);
    void leaving(void);
    void shift_right(void);
    void shift_left(void);
    void header(void);

    /*! This variable stores the copy of the function name. */
    SPtr<char> my_name;

    /*! If the caller function is a class member, this is the copy of the
        class name. */
    SPtr<char> my_class;

    /*! If the caller function is a class member, this is the class pointer
        (this). */
    const void *my_this;

#ifdef __GNUC__
    /*! */
    SPtr<char> my_filename;

    /*! */
    int my_lineno;
#endif

    class TabInfo
    {
        public:
            TabInfo(void);

            /*! This variable stores the current tabulation level (the number of nested
                function calls). */
            int tablevel;

            /*! */
            int tabshift;

            /*! */
            int id;

        private:
            static int next_id;
    };

    static std::map<pthread_t, TabInfo> levels;

    pthread_t actual_thread;

    TabInfo* info;

    /// This flag is set if the current module is on
    bool moduleIsOn;

    /// Hack to allocate with operator new[] instead of malloc
    /*! This function works like the system function strdup(), but allocates with operator new[].
        \note   It is added due to problems reported by valgrind. However, there is a mismatch of new and new[] yet,
                but it is not really a problem. */
    static inline char* StrDup(const char *s)
    {
        char *n = new char[strlen(s)+1];
        strcpy(n, s);
        return n;
    }

    /// The debug levels switched on
    static uint32_t debuglevel;

    /// The modules switched on
    static uint32_t debugmodules;

    static const char fill_1_begin[];
    static const char fill_1_end[];
    static const char fill_2[];
    static const char fill_right[];
    static const char fill_left[];
};


/*! This macro can be used to display the debug messages.<br>
    The main usage is:<br>
    <code>
    SYS_DEBUG(\<level\>, message [ << message ]...);
    </code>
    \param  level   The debuglevel of the message.
    \param  msg     The message to be printed. */
#if SYS_DEBUG_ON
#define SYS_DEBUG(level, msg)   \
    if (__debugprint.debug_module() && _DebugPrint::debug_print(level)) { \
        std::ostringstream __debug_temp_; \
        __debug_temp_ << msg;   \
        DEBUG_CRITICAL_SECTION; \
        __debugprint.draw_left(); \
        __debugprint << __debug_temp_; \
        __debugprint.endline(); \
    }
#else
#define SYS_DEBUG(level, msg) { }
#endif


/// Debug within a global function
/*! This macro defines a variable from the class _DebugPrint and
    passes the name of the function to the constructor. */
#if SYS_DEBUG_ON
#define SYS_DEBUG_FUNCTION(module)    _DebugPrint __debugprint(__FUNCTION__ __FILE_LINE__, module)
#else
#define SYS_DEBUG_FUNCTION(module) { }
#endif


/// Debug within a member function with given name
/*! This macro defines a variable from the class _DebugPrint and
    passes the name of the class and the function to the constructor. */
#if SYS_DEBUG_ON
#define SYS_DEBUG_MEMBER_NAME(__name) \
    _DebugPrint __debugprint(this, __name, __FUNCTION_STRING__ __FILE_LINE__, module)
#else
#define SYS_DEBUG_MEMBER_NAME(__name) { }
#endif


/// Debug within a member function
/*! This macro defines a variable from the class _DebugPrint and
    passes the name of the class and the function to the constructor. */
#if SYS_DEBUG_ON
#define SYS_DEBUG_MEMBER(module) \
    _DebugPrint __debugprint(this, className(), __FUNCTION_STRING__ __FILE_LINE__, module)
#else
#define SYS_DEBUG_MEMBER(module) { }
#endif


/// Debug within a static member function
/*! This macro defines a variable from the class _DebugPrint and
    passes the name of the class and the function to the constructor.<br>
    This version is for static member functions or for C functions. */
#if SYS_DEBUG_ON
#define SYS_DEBUG_STATIC(module) \
    _DebugPrint __debugprint(__FUNCTION_STRING__ __FILE_LINE__, module)
#else
#define SYS_DEBUG_STATIC(module) { }
#endif

#if SYS_DEBUG_ON
#define SYS_DEBUGLEVEL(level) _DebugPrint::set_level(level)
#else
#define SYS_DEBUGLEVEL(level) { }
#endif

#if SYS_DEBUG_ON
#define SYS_DEBUGMODULES(level) _DebugPrint::set_modules(level)
#else
#define SYS_DEBUGMODULES(level) { }
#endif

#if SYS_DEBUG_ON
#define IS_SYS_DEBUG(level) _DebugPrint::debug_print(level)
#else
#define IS_SYS_DEBUG(level)   false
#endif

#if SYS_DEBUG_ON
#define DBG_VIRTUAL     virtual
#else
#define DBG_VIRTUAL
#endif

#define YESNO(value)    ((value) ? "yes" : "no")

#endif // _DEBUGPRINT_H_

/* ------------------------- End - of - file --------------------------- */
