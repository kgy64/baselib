/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        ICExceptions.h
 * Purpose:     General purpose exceptions
 * Author:      Kövesdi György  (kgy@teledigit.eu)
 * Modified by:
 * Created:     2012-06-10
 * Licence:     GPL
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef _SYS_EXC_NAVI_H_
#define _SYS_EXC_NAVI_H_

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
    DEFINE_EXCEPTION(Assert, "Assert", BaseException);

    /// Uncorrectable error occured
    /*! Throwing this class closes the current operation but does not exit the program.
     *  */
    DEFINE_EXCEPTION(Error, "Error", Fatal);

    /// Problem during processing
    /*! Throwing this class exits the current operation in progress, but does
     *  not stop the current project.
     *  */
    DEFINE_EXCEPTION(Problem, "Problem", Error);

    /// Correctable problem during processing
    /*! Throwing this class means that some correctable error occured during processing, but it is
     *  not necessary to stop.
     *  */
    DEFINE_EXCEPTION(Continue, "Continue", Problem);
}

#define ASSERT(cond)    if (!(cond)) throw ::EX::Assert() << "'" << #cond << "' failed"

#endif // _SYS_EXC_NAVI_H_

/* ------------------------- End - of - file --------------------------- */
