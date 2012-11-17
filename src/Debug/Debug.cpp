/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        debugprint.cpp
 * Purpose:     Class to help debug prints.
 * Author:      Kövesdi György  (kgy@teledigit.eu)
 * Modified by:
 * Created:     2006-03-28
 * Licence:     GPL
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <System/Priority.h>
#include <Debug/Debug.h>
#include <iomanip>

using namespace std;

/*! */
map<pthread_t, _DebugPrint::TabInfo> INITIALIZE_PRIORITY_HIGH _DebugPrint::levels;


/*! */
Glib::Mutex INITIALIZE_PRIORITY_HIGH _DebugPrintLock::debugMutex;


/*! This is the current debuglevel. This value can be overwritten calling
    the _DebugPrint::set_level() function.
    \note   The list of levels must be defined by the parent project.
    \note   It is recommended to set it to zero here, while all the debuglevels can be switched
            on by using '-d' commandline option. */
uint32_t _DebugPrint::debuglevel = 0;

/*! This is the current list of modules switched on. This value can be overwritten calling
    the _DebugPrint::set_modules() function.
    \note   The list of modules must be defined by the parent project.
    \note   It is recommended to set it to zero here, while all the modules can be switched
            on by using '-m' commandline option. */
uint32_t _DebugPrint::debugmodules = 0;


/*! The _DebugPrint::entering() function starts the new message with
    this string. */
const char _DebugPrint::fill_1_begin[] = ",-{ ";


/*! The _DebugPrint::leaving() function starts the message with
    this string. */
const char _DebugPrint::fill_1_end[] = "`-} ";


/*! This is the separator string for each tabulation level. */
const char _DebugPrint::fill_2[] = "| ";

const char _DebugPrint::fill_right[] = " \\";

const char _DebugPrint::fill_left[] = " /";


/*! This constructor can be used from C or static C++ functions. The class name and 'this'
    pointer are not used.
    \param  name
    \param  module  */
#ifdef __GNUC__
/*! \param  fname
    \param  lineno */
#endif // __GNUC__
/*! \warning    Do <b>not</b> use this constructor directly, use the macro
                ::SYS_DEBUG_STATIC instead. */
_DebugPrint::_DebugPrint(const char *name,
#ifdef __GNUC__
                                const char *fname, int lineno,
#endif // __GNUC__
                                uint32_t module)
{
 moduleIsOn = module & get_modules();

 if (!moduleIsOn) {
    // Do not initialize anything: this class will do nothing (as few as possible)
    return;
 }

 my_name.reset(StrDup(name));
 my_class.reset(StrDup(""));
 my_this = NULL;
#ifdef __GNUC__
 my_filename.reset(StrDup(fname));
 my_lineno = lineno;
#endif // __GNUC__
 entering();
}


/*! This constructor can be used from 'C++' member functions. Prints the class name
    and 'this' pointer in the entering and leaving messages in addition to the
    function name.
    \param  thisptr     The class pointer of the caller.
    \param  classptr    The name of the caller class.
    \param  name        The name of the caller function.
    \param  module      The module mask. Each bit represents a module, therefore
                        32 modules can be controlled this way. A 'one' bit switches
                        on the corresponding module. */
#ifdef __GNUC__
/*! \param  fname
    \param  lineno */
#endif // __GNUC__
/*! \warning    Do <b>not</b> use this constructor deirectly, use the macro
                ::SYS_DEBUG_MEMBER instead. */
_DebugPrint::_DebugPrint(const void *thisptr, const char *classptr,
                                     const char *name,
#ifdef __GNUC__
                                    const char *fname, int lineno,
#endif // __GNUC__
                                    uint32_t module)
{
 moduleIsOn = module & get_modules();

 if (!moduleIsOn) {
    // Do not initialize anything: this class will do nothing (as few as possible)
    return;
 }

 my_name.reset(StrDup(name));
 my_class.reset(StrDup(classptr));
 my_this = thisptr;
#ifdef __GNUC__
 my_filename.reset(StrDup(fname));
 my_lineno = lineno;
#endif // __GNUC__
 entering();
}


/*! The destructor prints a leaving message for the actual function. */
_DebugPrint::~_DebugPrint()
{
 if (!moduleIsOn) {
    return;
 }

 leaving();
}


/*! This function prints a message for entering into a new function. */
void _DebugPrint::entering(void)
{
 DEBUG_CRITICAL_SECTION;

 actual_thread = pthread_self();
 info = &levels[actual_thread];

 shift_right();
 if (debug_print(DL_CALLS)) {
    draw_left();
    info->tablevel++;
    cerr << fill_1_begin;
    if (my_this != NULL) {
        cerr << &*my_class << "::" << &*my_name << "(): this=" << my_this << ", ";
    } else {
        cerr << &*my_name << "(): ";
    }
    cerr << &*my_filename << ":" << my_lineno << endl;
 } else {
    info->tablevel++;
 }
}


/*! This function prints a message for leaving the current function. */
void _DebugPrint::leaving(void)
{
 DEBUG_CRITICAL_SECTION;

 actual_thread = pthread_self();
 info = &levels[actual_thread];

 shift_left();
 if (debug_print(DL_CALLS)) {
    draw_left();
    cerr << fill_1_end;
    if (my_this != NULL)
        cerr << &*my_class << "::";
    cerr << &*my_name << "()" << endl;
 }
}


void _DebugPrint::shift_right(void)
{
 if (info->tablevel > 20) {
    if (debug_print(DL_CALLS)) {
#if 0
        cerr << "----<----<----<------- Nesting too deep, shifting left 10 positions "
            "----<----<----<----<---\n";
#else
        for (int j = 0; j < 10; j++) {
            info->tablevel--;
            header();
            cerr << "/";
            for (int i = 0; i<info->tablevel; i++) cerr << fill_left;
            cerr << endl;
            header();
            for (int i = 0; i<info->tablevel; i++) cerr << fill_left;
            cerr << endl;
        }
#endif
    }
    info->tabshift++;
 }
}


void _DebugPrint::shift_left(void)
{
 --info->tablevel;
 if (info->tabshift > 0 && info->tablevel <= 5) {
    if (debug_print(DL_CALLS)) {
#if 0
        cerr << "---->---->---->---- Nesting gets shallow, shifting right 10 positions "
            "-->---->---->---->----\n";
#else
        for (int j = 0; j < 10; j++) {
            header();
            cerr << "\\";
            for (int i = 0; i<info->tablevel; i++) cerr << fill_right;
            cerr << endl;
            header();
            for (int i = -1; i<info->tablevel; i++) cerr << fill_right;
            cerr << endl;
            info->tablevel++;
        }
#endif
    }
    --info->tabshift;
 }
}


/*! This function prints the separator string (stored in the
    _DebugPrint::fill_2) TabInfo::tablevel times. */
void _DebugPrint::draw_left(void)
{
 header();
 if (debug_print(DL_CALLS)) for (int i=0; i<info->tablevel; i++) cerr << fill_2;
}


void _DebugPrint::header(void)
{
 cerr << setw(3) << info->id << ": ";
}


_DebugPrint& _DebugPrint::operator<<(const std::ostringstream& p_string)
{
 cerr << p_string.str();
 return *this;
}


void _DebugPrint::endline(void)
{
 cerr << endl;
}


int _DebugPrint::TabInfo::next_id = 0;

_DebugPrint::TabInfo::TabInfo(void): tablevel(0), tabshift(0), id(next_id++)
{
}

/* ------------------------- End - of - file --------------------------- */
