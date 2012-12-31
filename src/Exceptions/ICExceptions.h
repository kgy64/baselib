/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        ICExceptions.h
 * Purpose:     General purpose exceptions
 * Author:      Kövesdi György  (kgy@teledigit.eu)
 * Modified by:
 * Created:     2012-06-10
 * Licence:     GPL
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef _EXCEPTIONS_EXCEPTIONS_H_
#define _EXCEPTIONS_EXCEPTIONS_H_

#include <Debug/Debug.h>
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
     private:                               \
        SYS_DEFINE_CLASS_NAME("EX::" MSG "Exception"); \
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

     private:
        SYS_DEFINE_CLASS_NAME("EX::BaseException");

        std::string os;
    };

    /// Fatal exception
    /*! Throwing this class anywhere in the program the whole system exits. This is the
     *  possible highest level of exceptions.
     *  */
    DEFINE_EXCEPTION(Fatal, "Fatal", BaseException);

    /// Assert failed
    /*! This exception is tied to boost assertion, if BOOST_ENABLE_ASSERT_HANDLER is defined.
     *  Basically it is a fatal exception but can be caught in some special cases.<br>
     *  Also thrown by the macros \ref ASSERT and \ref ASSERT_DBG
     *  */
    DEFINE_EXCEPTION(Assert, "Assert", BaseException);

    /// Uncorrectable error occured
    /*! Throwing this class closes the current operation but it can continue if possible.
     *  */
    DEFINE_EXCEPTION(Error, "Error", Fatal);

    /// Correctable problem during processing
    /*! Throwing this class means that some correctable error occured during processing, but it is
     *  not necessary to stop.
     *  */
    DEFINE_EXCEPTION(Continue, "Continue", Error);
}

#define __DO_ASSERT(cond, message)  \
    throw ::EX::Assert() << "In file '" __FILE__ "', line " << __LINE__ << ": '" #cond "' failed: " << message

#define ASSERT(cond, message)   { if (!(cond)) { __DO_ASSERT(cond, message); } }

#define ASSERT_DBG(cond, message) \
    { \
        if (!(cond)) { \
            SYS_DEBUG(DL_ASSERT, "Line " << __LINE__ << ": Assert '" #cond "' is failed here"); \
            __DO_ASSERT(cond, message); \
        } \
    }

#define ASSERT_STD(cond)    if (!(cond)) __DO_ASSERT(cond, strerror(errno))

#endif // _EXCEPTIONS_EXCEPTIONS_H_

/* ------------------------- End - of - file --------------------------- */
