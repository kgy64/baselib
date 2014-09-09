/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        debugprint.cpp
 * Purpose:     Class to help debug prints.
 * Author:      Kövesdi György  (kgy@teledigit.eu)
 * Modified by:
 * Created:     2006-03-28
 * Licence:     GPL
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "Debug.h"

#include <Threads/Threads.h>
#include <System/Generic.h>

#include <iomanip>

::_Debug_Info_::_Debug_Module_ Debug::__builtin_debug_module(true);

using namespace std;
using namespace _Debug_Info_;

AUTON_INTERFACE(I_DebugOut);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                                       *
 *         class _Debug_Info_::_All_Modules_:                                            *
 *                                                                                       *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

_Debug_Module_ * _All_Modules_::first = (_Debug_Module_*)0;

void _All_Modules_::SetMode(bool mode)
{
 for (_Debug_Module_ * mod = first; mod; mod=mod->next) {
    mod->SetMode(mode);
 }
}

void _All_Modules_::SetDebuglevel(unsigned int level)
{
 for (_Debug_Module_ * mod = first; mod; mod=mod->next) {
    mod->SetDebuglevel(level);
 }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                                       *
 *         class _Debug_Info_::DebugPrint:                                               *
 *                                                                                       *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

boost::scoped_ptr<Auton<I_DebugOut> > _Debug_Info_::DebugPrint::out_stream;

/*! */
map<pthread_t, DebugPrint::TabInfo> INITIALIZE_PRIORITY_HIGH DebugPrint::levels;

/// Mutex for Debug Output
/*! */
Threads::Mutex INITIALIZE_PRIORITY_HIGH PrintLock::debugMutex;

/*! The DebugPrint::entering() function starts the new message with
    this string. */
const char DebugPrint::fill_1_begin[] = ",-{ ";

/*! The DebugPrint::leaving() function starts the message with
    this string. */
const char DebugPrint::fill_1_end[] = "`-} ";

/*! This is the separator string for each tabulation level. */
const char DebugPrint::fill_2[] = "| ";

const char DebugPrint::fill_right[] = " \\";

const char DebugPrint::fill_left[] = " /";

/*! This constructor can be used from C or static C++ functions. The class name and 'this'
    pointer are not used.
    \param  name
    \param  p_module  */
/*! \param  fname
    \param  lineno */
/*! \warning    Do <b>not</b> use this constructor directly, use the macro
                ::SYS_DEBUG_STATIC instead. */
DebugPrint::DebugPrint(const char *name, const char *fname, int lineno, ::_Debug_Info_::_Debug_Module_ & p_module):
    my_this(NULL),
    my_lineno(lineno),
    info(NULL),
    myModule(p_module)
{
 if (!myModule.IsOn()) {
    // Do not initialize anything: this class will do nothing (or at least as few as possible)
    // Note that the member 'info' remains NULL in this case.
    return;
 }

 info = &levels[Threads::getTid()];

 if (name) {
    my_name.reset(StrDup(name));
 }
 my_class.reset(StrDup(""));
 my_filename.reset(StrDup(fname));

 if (my_name) {
    entering();
 }
}


/*! This constructor can be used from 'C++' member functions. Prints the class name
    and 'this' pointer in the entering and leaving messages in addition to the
    function name.
    \param  thisptr     The class pointer of the caller.
    \param  classptr    The name of the caller class.
    \param  name        The name of the caller function.
    \param  p_module    Reference of the actual module. */
/*! \param  fname
    \param  lineno */
/*! \warning    Do <b>not</b> use this constructor deirectly, use the macro
                ::SYS_DEBUG_MEMBER instead. */
DebugPrint::DebugPrint(const void *thisptr, const char *classptr, const char *name, const char *fname, int lineno, ::_Debug_Info_::_Debug_Module_ & p_module):
    my_this(thisptr),
    my_lineno(lineno),
    info(NULL),
    myModule(p_module)
{
 if (!myModule.IsOn()) {
    // Do not initialize anything: this class will do nothing (or at least as few as possible)
    // Note that the member 'info' remains NULL in this case.
    return;
 }

 info = &levels[Threads::getTid()];

 if (name) {
    my_name.reset(StrDup(name));
 }

 my_class.reset(StrDup(classptr));
 my_filename.reset(StrDup(fname));

 if (my_name) {
    entering();
 }
}


/*! The destructor prints a leaving message for the actual function. */
DebugPrint::~DebugPrint()
{
 if (!myModule.IsOn()) {
    // The class has not been initialized: do nothing here
    return;
 }

 if (my_name) {
    leaving();
 }
}


/*! This function prints a message for entering into a new function. */
void DebugPrint::entering(void)
{
 DEBUG_CRITICAL_SECTION;

 if (!level_is_on(DL_CALLS)) {
    return;
 }

 I_DebugOut & out(GetOutStream());

 shift_right();
 draw_left();
 out << fill_1_begin;

 if (my_name) {
    if (my_this != NULL) {
        out << my_class.get() << "::" << my_name.get() << "(): this=" << my_this << ", ";
    } else {
        out << my_name.get() << "(): ";
    }
 }

 out << my_filename.get() << ":" << my_lineno;
 endline();

 info->tablevel++;
}


/*! This function prints a message for leaving the current function. */
void DebugPrint::leaving(void)
{
 if (!level_is_on(DL_CALLS)) {
    return;
 }

 DEBUG_CRITICAL_SECTION;

 I_DebugOut & out(GetOutStream());

 shift_left();
 draw_left();
 out << fill_1_end;

 if (my_this != NULL) {
    out << my_class.get() << "::";
 }

 out << my_name.get() << "()";
 endline();
}


void DebugPrint::shift_right(void)
{
 if (info->tablevel > 20) {
    if (level_is_on(DL_CALLS)) {
        I_DebugOut & out(GetOutStream());
        for (int j = 0; j < 10; j++) {
            --info->tablevel;
            overture();
            out << "/";
            for (int i = 0; i<info->tablevel; i++) out << fill_left;
            endline();
            overture();
            for (int i = 0; i<info->tablevel; i++) out << fill_left;
            endline();
        }
    }
    info->tabshift++;
 }
}


void DebugPrint::shift_left(void)
{
 --info->tablevel;
 if (info->tabshift > 0 && info->tablevel <= 5) {
    if (level_is_on(DL_CALLS)) {
        I_DebugOut & out(GetOutStream());
        for (int j = 0; j < 10; j++) {
            overture();
            out << "\\";
            for (int i = 0; i<info->tablevel; i++) out << fill_right;
            endline();
            overture();
            for (int i = -1; i<info->tablevel; i++) out << fill_right;
            endline();
            ++info->tablevel;
        }
    }
    --info->tabshift;
 }
}


/*! This function prints the separator string (stored in the
    DebugPrint::fill_2) TabInfo::tablevel times. */
void DebugPrint::draw_left(bool separators)
{
 overture();
 if (level_is_on(DL_CALLS)) {
    I_DebugOut & out(GetOutStream());
    out.header();
    if (separators) {
        for (int i=0; i<info->tablevel; i++) {
            out << fill_2;
        }
    }
 }
}


void DebugPrint::overture(void)
{
 char id_string[40];
 sprintf(id_string, " %5d: ", info->tid);
 GetOutStream() << id_string;
}


DebugPrint& DebugPrint::operator<<(const std::ostringstream& p_string)
{
 GetOutStream() << p_string.str().c_str();
 return *this;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                                       *
 *         class _Debug_Info_::DebugPrint::TabInfo:                                      *
 *                                                                                       *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int DebugPrint::TabInfo::next_id = 0;

DebugPrint::TabInfo::TabInfo(void):
    printlevel(0),
    tablevel(0),
    tabshift(0),
    id(next_id++),
    tid(Threads::getTid())
{
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 *                                                                                       *
 *         class I_DebugOut:                                                             *
 *                                                                                       *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

I_DebugOut & I_DebugOut::operator<<(const void * p)
{
 char tmp[30];
 sprintf(tmp, "%p", p);
 return *this << tmp;
}

I_DebugOut & I_DebugOut::operator<<(int data)
{
 char tmp[30];
 sprintf(tmp, "%d", (unsigned)data);
 return *this << tmp;
}

/* ------------------------- End - of - file --------------------------- */
