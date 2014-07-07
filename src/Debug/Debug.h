/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        Debug.h
 * Purpose:     Class for debugging
 * Author:      Kövesdi György  (kgy@teledigit.eu)
 * Modified by: KGy 2013-05-31: The stdc++-based version
 *              KGy 2013-10-20: Customizable output stream
 * Created:     2006-03-28
 * Licence:     GPL
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __SRC_DEBUG_DEBUG_H_INCLUDED__
#define __SRC_DEBUG_DEBUG_H_INCLUDED__

#include <boost/scoped_array.hpp>
#include <boost/scoped_ptr.hpp>
#include <iostream>
#include <sstream>
#include <map>
#include <string.h>
#include <stdint.h>

#include <Threads/Mutex.h>
#include <Memory/Auton.h>

#include "debuglevels.h" // Note: this must be provided by the user!

#define _SYS_DEBUG_MODULE_NAME(name)   __debug_module_##name

#define CLASS_NAME_FUNCTION     __Class_Name

class I_DebugOut
{
 public:
    virtual ~I_DebugOut()
    {
    }

    virtual void header(void)
    {
    }

    virtual I_DebugOut & operator<<(const char *) =0;
    virtual I_DebugOut & operator<<(const void *);
    virtual I_DebugOut & operator<<(int);

 protected:
    inline I_DebugOut(void)
    {
    }

}; // class I_DebugOut

namespace _Debug_Info_
{
    class _Debug_Module_;

    class _All_Modules_
    {
     friend class _Debug_Module_;
     public:
        static void SetMode(bool mode);
        static void SetDebuglevel(unsigned int level);
     private:
        static _Debug_Module_ * first;
    };

    class _Debug_Module_
    {
     friend class _All_Modules_;
     public:
        _Debug_Module_(void):
            debuglevel(0UL),
            is_on(false)
        {
            next = _All_Modules_::first;
            _All_Modules_::first = this;
        }

        void SetMode(bool mode)
        {
            is_on = mode;
        }

        bool IsOn(void)
        {
            return is_on;
        }

        void SetDebuglevel(unsigned int level)
        {
            debuglevel = level;
        }

        bool level_is_on(_GenericDebugLevels level)
        {
            return IsOn() && (debuglevel & level);
        }

     private:
        unsigned int debuglevel;
        _Debug_Module_ * next;
        bool is_on;
    };
}

#define SYS_GET_CLASS_NAME  CLASS_NAME_FUNCTION()

#if SYS_DEBUG_ON
#define SYS_DEFINE_CLASS_NAME(name) \
    virtual const char* CLASS_NAME_FUNCTION(void) const { return name; }
#else
#define SYS_DEFINE_CLASS_NAME(name)
#endif

#define DEBUG_CRITICAL_SECTION  ::_Debug_Info_::PrintLock __lock_

namespace _Debug_Info_
{
    class PrintLock: public Threads::Lock
    {
     public:
        inline PrintLock(void):
            Threads::Lock(debugMutex)
        {
        }

     private:
        static Threads::Mutex debugMutex;

    }; // class _Debug_Info_::PrintLock

    /// Debug message handler class
    /*! This class is responsible for handling debug messages. The messages can show
        the levels of nested function calls if DL_CALLS is set. If the functions are
        class members, the class name and 'this' pointer is also displayed. To do this,
        those classes (where the debug messages are printed from) must have a function
        to reach their class name (called __Class_Name()).<br>
        This class can be fine-tuned by changing ::_Debug_Info_::DebugPrint::fill_1_begin,
        ::_Debug_Info_::DebugPrint::fill_1_end, ::_Debug_Info_::DebugPrint::fill_2 strngs.<br>
        <b>Usage:</b><br>
        Such a class can be constructed at the beginning of a function, as a
        local variable. The constructor and the destructor prints the 'entering...'
        and 'leaving...' messages. See the macros ::SYS_DEBUG_FUNCTION,
        ::SYS_DEBUG_MEMBER and ::SYS_DEBUG_STATIC on how does it work. <br>
        The ::_Debug_Info_::DebugPrint::level_is_on() function can be used to determine if the
        current message shall be printed or not. See the macro ::SYS_DEBUG for details.<br>
        The messages are pinted in semi-graphic format, to be able to follow the
        program flow easily this way.
        \warning    It is not possible to use the debug macros from this class
                    because it would lead to deadlock or endless loop. */
    class DebugPrint
    {
     public:
        DebugPrint(const char *name, const char *fname, int lineno, ::_Debug_Info_::_Debug_Module_ & p_module);
        DebugPrint(const void *thisptr, const char *classptr, const char *name, const char *fname, int lineno, ::_Debug_Info_::_Debug_Module_ & p_module);
        ~DebugPrint();

        void draw_left(void);
        void endline(void);

        /*! This function just decides if the message shall be printed or not. */
        inline bool level_is_on(_GenericDebugLevels level) {
            return info->printlevel == 0 && myModule.level_is_on(level);
        }

        inline void EnterDebugPrint(void)
        {
            ++info->printlevel;
        }

        inline void LeaveDebugPrint(void)
        {
            --info->printlevel;
        }

        DebugPrint& operator<<(const std::ostringstream& p_string);

     private:
        void entering(void);
        void leaving(void);
        void shift_right(void);
        void shift_left(void);
        void overture(void);

        /*! This variable stores the copy of the function name. */
        boost::scoped_array<char> my_name;

        /*! If the caller function is a class member, this is the copy of the
            class name. */
        boost::scoped_array<char> my_class;

        /*! If the caller function is a class member, this is the class pointer
            (this). */
        const void *my_this;

        /*! */
        boost::scoped_array<char> my_filename;

        /*! */
        int my_lineno;

        class TabInfo
        {
         public:
            TabInfo(void);

            /*! This variable is incremented when a DEBUG macro starts assembling its message to
             *  prevent printing unnecessary debug messages during message assembly. */
            int printlevel;

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

        TabInfo* info;

        /// Hack to allocate with operator new[] instead of malloc
        /*! This function works like the system function strdup(), but allocates with operator new[].
            \note   It is added due to problems reported by valgrind. */
        static inline char* StrDup(const char *s)
        {
            char *n = new char[strlen(s)+1];
            strcpy(n, s);
            return n;
        }

        ::_Debug_Info_::_Debug_Module_ & myModule;

        static const char fill_1_begin[];
        static const char fill_1_end[];
        static const char fill_2[];
        static const char fill_right[];
        static const char fill_left[];

        static boost::scoped_ptr<Auton<I_DebugOut> > out_stream;

        inline static I_DebugOut & GetOutStream(void)
        {
            if (!out_stream) {
                out_stream.reset(new Auton<I_DebugOut>);
            }
            return **out_stream;
        }

    }; // class _Debug_Info_::DebugPrint

    class DebugFilter
    {
     public:
        inline DebugFilter(DebugPrint & parent):
            parent(parent)
        {
            parent.EnterDebugPrint();
        }

        inline ~DebugFilter()
        {
            parent.LeaveDebugPrint();
        }

     private:
        DebugPrint & parent;

    }; // class _Debug_Info_::DebugFilter

} // namespace _Debug_Info_

/// Print a message unconditionally
#define DEBUG_OUT(msg)                                  \
    {                                                   \
        std::ostringstream __debug_temp_;               \
        __debug_temp_ << msg << std::endl;              \
        Auton<I_DebugOut> __debug_out;                  \
        (*__debug_out) << __debug_temp_.str().c_str();  \
    }

/*! This macro switches the debug output temporarily off. */
#if SYS_DEBUG_ON
#define SYS_DEBUG_OFF   _Debug_Info_::DebugFilter   __switch_debug_off(__debugprint)
#else
#define SYS_DEBUG_OFF   { }
#endif

/*! This macro can be used to display the debug messages.<br>
    The main usage is:<br>
    <code>
    SYS_DEBUG(\<level\>, message [ << message ]...);
    </code>
    \param  level   The debuglevel of the message.
    \param  msg     The message to be printed.
    \note   The message itself must be built before entering the critical section to prevent deadlock
            by calling member functions recursively from here. */
#if SYS_DEBUG_ON
#define SYS_DEBUG(level, msg)   \
    if (__debugprint.level_is_on(level)) { \
        std::ostringstream __debug_temp_; \
        { \
            SYS_DEBUG_OFF; \
            __debug_temp_ << msg;   \
        } \
        DEBUG_CRITICAL_SECTION; \
        __debugprint.draw_left(); \
        __debugprint << __debug_temp_; \
        __debugprint.endline(); \
    }
#else
#define SYS_DEBUG(level, msg) { }
#endif

/// Debug within a global function
/*! This macro defines a variable from the class ::_Debug_Info_::DebugPrint and
    passes the name of the function to the constructor. */
#if SYS_DEBUG_ON
#define SYS_DEBUG_FUNCTION(module_name) \
    ::_Debug_Info_::DebugPrint __debugprint(__FUNCTION__, __FILE__, __LINE__, _SYS_DEBUG_MODULE_NAME(module_name))
#else
#define SYS_DEBUG_FUNCTION(name) { }
#endif

/// Debug within a static member function
/*! Currently it is the same as SYS_DEBUG_FUNCTION
 */
#define SYS_DEBUG_STATIC(name) SYS_DEBUG_FUNCTION(name)

/// Debug within a member function with given name
/*! This macro defines a variable from the class ::_Debug_Info_::DebugPrint and
    passes the name of the class and the function to the constructor. */
#if SYS_DEBUG_ON
#define SYS_DEBUG_MEMBER_NAME(name, module_name) \
    ::_Debug_Info_::DebugPrint __debugprint(this, name, __FUNCTION__, __FILE__, __LINE__, _SYS_DEBUG_MODULE_NAME(module_name))
#else
#define SYS_DEBUG_MEMBER_NAME(name, module_name) { }
#endif

/// Debug within a member function
/*! This macro defines a variable from the class ::_Debug_Info_::DebugPrint and
    passes the name of the class and the function to the constructor. */
#if SYS_DEBUG_ON
#define SYS_DEBUG_MEMBER(module_name) \
    ::_Debug_Info_::DebugPrint __debugprint(this, CLASS_NAME_FUNCTION(), __FUNCTION__, __FILE__, __LINE__, _SYS_DEBUG_MODULE_NAME(module_name))
#else
#define SYS_DEBUG_MEMBER(module_name) { }
#endif

#if SYS_DEBUG_ON
#define SYS_DEBUGLEVEL(level) ::_Debug_Info_::_All_Modules_::SetDebuglevel(level)
#else
#define SYS_DEBUGLEVEL(level) { }
#endif

#if SYS_DEBUG_ON
#define SYS_DEBUGLEVEL_MODULE(name, level) _SYS_DEBUG_MODULE_NAME(name).SetDebuglevel(level)
#else
#define SYS_DEBUGLEVEL_MODULE(name, level) { }
#endif

#if SYS_DEBUG_ON
#define SYS_DEBUG_MODULE(name, onoff) _SYS_DEBUG_MODULE_NAME(name).SetMode(onoff)
#else
#define SYS_DEBUG_MODULE(name, onoff) { }
#endif

#define SYS_DEBUG_MODULE_ON(name) SYS_DEBUG_MODULE(name, true)

#define SYS_DEBUG_MODULE_OFF(name) SYS_DEBUG_MODULE(name, false)

#if SYS_DEBUG_ON
#define SYS_DEBUG_ALL_MODULES(onoff) ::_Debug_Info_::_All_Modules_::SetMode(onoff)
#else
#define SYS_DEBUG_ALL_MODULES(onoff)
#endif

#define SYS_DEBUG_ALL_MODULES_ON SYS_DEBUG_ALL_MODULES(true)

#define SYS_DEBUG_ALL_MODULES_OFF SYS_DEBUG_ALL_MODULES_OFF(false)

#if SYS_DEBUG_ON
#define SYS_DEFINE_MODULE(name) ::_Debug_Info_::_Debug_Module_ _SYS_DEBUG_MODULE_NAME(name)
#define SYS_DECLARE_MODULE(name) extern SYS_DEFINE_MODULE(name)
#else
#define SYS_DECLARE_MODULE(name)
#define SYS_DEFINE_MODULE(name)
#endif

#if SYS_DEBUG_ON
#define VIRTUAL_IF_DEBUG    virtual
#else
#define VIRTUAL_IF_DEBUG
#endif

#define YESNO(value)    ((value) ? "yes" : "no")

#endif // __SRC_DEBUG_DEBUG_H_INCLUDED__

/* ------------------------- End - of - file --------------------------- */
