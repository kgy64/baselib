/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        Exceptions.h
 * Purpose:     General purpose exceptions
 * Author:      Kövesdi György  (kgy@teledigit.eu)
 * Modified by:
 * Created:     2012-06-10
 * Licence:     GPL
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef _EXCEPTIONS_EXCEPTIONS_H_
#define _EXCEPTIONS_EXCEPTIONS_H_

#include <string.h> // for strerror()
#include <exception>
#include <iostream>
#include <sstream>

#define DEFINE_EXCEPTION(NAME, MSG, BASE)   \
    class NAME: public BASE                 \
    {                                       \
     public:                                \
        NAME(const char * msg = MSG, int line = -1): \
            BASE(msg, line)                 \
        {                                   \
        }                                   \
                                            \
        virtual ~NAME() throw()             \
        {                                   \
        }                                   \
                                            \
        template <typename T>               \
        inline NAME & operator<<(const T & value)  \
        {                                   \
            static_cast<BASE &>(*this) << value;   \
            return *this;                   \
        }                                   \
                                            \
    };

namespace EX {
    class BaseException: public std::exception
    {
     public:
        BaseException(const char * msg = NULL, int line = -1);
        virtual ~BaseException() throw();

        virtual const char * what(void) const throw()
        {
            return os.c_str();
        }

        template <typename T>
        inline BaseException & operator<<(const T & value)
        {
            std::ostringstream s;
            s << value;
            os += s.str();
            return *this;
        }

        std::string os;
    };

    /// Fatal exception
    /*! Throwing this class anywhere in the program the whole system exits. This is the
     *  possible highest level of exceptions.
     *  */
    DEFINE_EXCEPTION(Fatal, "Fatal Exception", BaseException);

    /// Uncorrectable error occured
    /*! Throwing this class closes the current operation but it can continue if possible.
     *  */
    DEFINE_EXCEPTION(Error, "Error", Fatal);

    /// Assert failed
    /*! This exception is tied to boost assertion, if BOOST_ENABLE_ASSERT_HANDLER is defined.
     *  Basically it is a fatal exception but can be caught in some special cases.<br>
     *  Also thrown by the macros \ref ASSERT and \ref ASSERT_DBG
     *  */
    DEFINE_EXCEPTION(Assert, "Assert", Error);

    /// Less critical problem occured
    /*! This exception probably prints a warning message and continues the operation. */
    DEFINE_EXCEPTION(Warning, "Warning", Error);

    /// Stop operation and continue
    /*! This exception means that the current operation must be stopped, but it is not really
     *  an error and no further actions needed. The operation can continue. */
    DEFINE_EXCEPTION(Continue, NULL, Warning);
}

#define __DO_ASSERT(type, cond, message)  \
    throw type("assertion '" #cond "' failed in file " __FILE__, __LINE__) << message

#define ASSERT_T(type, cond, message)   { if (!(cond)) { __DO_ASSERT(type, cond, message); } }

#define ASSERT(cond, message)   ASSERT_T(::EX::Assert, cond, message)

#define ASSERT_FATAL(cond, message) ASSERT_T(::EX::Fatal, cond, message)

#define ASSERT_DBG(cond, message) \
    { \
        if (!(cond)) { \
            SYS_DEBUG(DL_ASSERT, "Assert '" #cond "' failed: " << message); \
            __DO_ASSERT(::EX::Assert, cond, message); \
        } \
    }

#define ASSERT_STD(cond)                    { if (!(cond)) __DO_ASSERT(::EX::Assert, cond, strerror(errno)); }

#define ASSERT_STD_ERRNO(cond, error_code)  { if (!(cond)) __DO_ASSERT(::EX::Assert, cond, strerror(error_code)); }

#define ASSERT_STRERROR(cond, message)      { if (!(cond)) __DO_ASSERT(::EX::Assert, cond, message << strerror(errno)); }

#define ASSERT_STD_ZERO(cond)               { int e = (cond); if (e) __DO_ASSERT(::EX::Assert, cond, strerror(e)); }

#endif // _EXCEPTIONS_EXCEPTIONS_H_

/* ------------------------- End - of - file --------------------------- */
