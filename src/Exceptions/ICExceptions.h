/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Name:        ICExceptions.h
 * Purpose:     Navi-specific exceptions
 * Author:      Kövesdi György  (kgy@teledigit.eu)
 * Modified by:
 * Created:     2012-06-10
 * Licence:     GPL
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef _SYS_EXC_NAVI_H_
#define _SYS_EXC_NAVI_H_

#include <Debug/Debug.h>
#include <glibmm/exception.h>

namespace EX {
    class BaseException: public Glib::Exception
    {
        public:
            BaseException(const char * msg):
                message(msg)
            {
            }

            virtual ~BaseException() throw()
            {
            }

            virtual Glib::ustring what(void) const
            {
                return message;
            }

        private:
            SYS_DEFINE_CLASS_NAME("EX::BaseException");

            const char * message;
    };

    /// Fatal exception
    /*! Throwing this class anywhere in the program the whole Navi system exits. This is the
     *  possible highest level of exceptions.
     *  */
    class Fatal: public BaseException
    {
        public:
            Fatal(const char * message): BaseException(message) { }

        private:
            SYS_DEFINE_CLASS_NAME("EX::Fatal");
    };

    /// Uncorrectable error occured
    /*! Throwing this class closes the current operation but does not exit the Navi program.
     *  */
    class Error: public Fatal
    {
        public:
            Error(const char * message): Fatal(message) { }

        private:
            SYS_DEFINE_CLASS_NAME("EX::Error");
    };

    /// Problem during processing
    /*! Throwing this class exits the current operation in progress, but does
     *  not stop the current project.
     *  */
    class Problem: public Error
    {
        public:
            Problem(const char * message): Error(message) { }

        private:
            SYS_DEFINE_CLASS_NAME("EX::Problem");
    };

    /// Correctable problem during processing
    /*! Throwing this class means that some correctable error occured during processing, but it is
     *  not necessary to stop.
     *  */
    class Continue: public Problem
    {
        public:
            Continue(const char * message): Problem(message) { }

        private:
            SYS_DEFINE_CLASS_NAME("EX::Continue");
    };
}

#endif // _SYS_EXC_NAVI_H_

/* ------------------------- End - of - file --------------------------- */
