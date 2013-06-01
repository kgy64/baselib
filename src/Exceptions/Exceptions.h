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
        NAME(const char * msg = MSG " Exception", int line = -1): \
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
        BaseException(const char * msg = "Generic Exception", int line = -1);
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
    DEFINE_EXCEPTION(Fatal, "Fatal", BaseException);

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

    /// Correctable problem during processing
    /*! Throwing this class means that some correctable error occured during processing, but it is
     *  not necessary to stop.
     *  */
    DEFINE_EXCEPTION(Continue, "Continue", Error);
}

#define __DO_ASSERT(type, cond, message)  \
    throw type() << "In file " __FILE__ ":" << __LINE__ << ": '" #cond "' failed: " << message

#define ASSERT_T(type, cond, message)   { if (!(cond)) { __DO_ASSERT(type, cond, message); } }

#define ASSERT(cond, message)   ASSERT_T(::EX::Assert, cond, message)

#define ASSERT_FATAL(cond, message) ASSERT_T(::EX::Fatal, cond, message)

#define ASSERT_DBG(cond, message) \
    { \
        if (!(cond)) { \
            SYS_DEBUG(DL_ASSERT, __FILE__ << ":" << __LINE__ << ": Assert '" #cond "' is failed here; " << message); \
            __DO_ASSERT(::EX::Assert, cond, message); \
        } \
    }

#define ASSERT_STD(cond)    if (!(cond)) __DO_ASSERT(::EX::Assert, cond, strerror(errno))

#define ASSERT_STD_ERRNO(cond, error_code)    if (!(cond)) __DO_ASSERT(::EX::Assert, cond, strerror(error_code))

#define ASSERT_STD_ZERO(cond)   { int e = (cond); if (e) __DO_ASSERT(::EX::Assert, cond, strerror(e)); }

#endif // _EXCEPTIONS_EXCEPTIONS_H_

/* ------------------------- End - of - file --------------------------- */
