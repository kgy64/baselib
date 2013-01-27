/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Project:     General Purpose Library
 * Purpose:     Class Auton is a smart pointer to instantiate a singleton
 *              using its interface
 * Author:      György Kövesdi <kgy@teledigit.eu>
 * Licence:     GPL (see file 'COPYING' in the project root for more details)
 * Comments:    
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __AUTON_H__
#define __AUTON_H__

#include <string.h>

#include <System/Generic.h>
#include <Exceptions/ICExceptions.h>
#include <Debug/Debug.h>

SYS_DECLARE_MODULE(DM_AUTON);

// Note that in this file 'I' means the Interface class, while 'C' means the Implementation

// Predeclarations:
template <class I> class Auton;
template <class I> class AutoImp;

/// Specific namespace for \ref Auton and related things
/*! This namespace contains everything related to \ref Auton to prevent any interference
    with other stuff.
 */
namespace _AutonPrivate
{
    // Predeclarations:
    template <class I> class AutonBase;

    /// Helper class for Interfaces
    /*! Such a class is instantiated for each Interface by the macro ::AUTON_INTERFACE
        (see other details there).<br>
        Such an instance will handle the Implementation requests via its function \ref
        AutonInterface::_RegisterImplementation()<br>
        This class creates the corresponding Interface implementation on demand: the
        first access to the Interface will instantiate the corresponding Implementation.<br>
        This class has a reference counter and deletes the Implementation if the last
        referenced pointer (\ref Auton) is deleted.
     */
    template <class I>
    class AutonInterface
    {
        /*! The class \ref AutonBase accesses its \ref head and \ref _RegisterImplementation()
            members during static initialization time.
         */
        friend class AutonBase<I>;

        /*! The class \ref AutoImp has right to access its member functions during runtime.
         */
        friend class AutoImp<I>;

        /*! The class \ref Auton has right to access its member functions during runtime.
         */
        friend class Auton<I>;

     public:
        AutonInterface(const char * p_name)
        {
            if (!myTypeName) {
                SetName(p_name);
            }
        }

        /*! Because this helper class is created as a static variable, it is deleted
            only during exit, after the main() function has been returned. The Interface
            instance will be deleted here, if any.
         */
        virtual ~AutonInterface()
        {
            delete myInterface;
        }

        // *** Has already been commented, see below *** //
        bool _ForceImplementation(const char * typeName);

     private:
        SYS_DEFINE_CLASS_NAME(myName);

        void SetName(const char * p_name)
        {
            myTypeName = p_name;
            snprintf(myName, sizeof(myName), "AutonInterface<%s>", myTypeName);
            myName[sizeof(myName)-1] = '\0';
        }

        void SetImplementation(AutonBase<I> * p_impl)
        {
            implementation = p_impl;
            SetName(p_impl->typeName);
        }

        /// Always creates a new Implementation
        /*! \note It is useful for class AutoImp
         */
        inline I * CreateImplementation(void)
        {
            ASSERT_FATAL(implementation, "Interface class '" << myTypeName << "' is not defined");
            return implementation->Create();
        }

        /// Checks if the Implementation is instantiated or not
        inline bool IsValid(void)
        {
            return myInterface;
        }

        /// Increments the reference counter
        /*! \see AutonInterface::references
            \note   It is called by each constructor of the corresponding type
         */
        inline void Use(void)
        {
            SYS_DEBUG_MEMBER(DM_AUTON);
            Glib::Mutex::Lock _l(myMutex);
            ++references;
            SYS_DEBUG(DL_INFO1, "The usage conunter of '" << myName << "' is now " << references);
        }

        /// Decrements the reference counter
        /*! Also deletes the Implementation if it was the last usage.
            \see AutonInterface::references
            \note   It is called by each destructor of the corresponding type
         */
        inline void Drop(void)
        {
            SYS_DEBUG_MEMBER(DM_AUTON);
            Glib::Mutex::Lock _l(myMutex);
            SYS_DEBUG(DL_INFO1, "The usage conunter of '" << myName << "' was " << references);
            if (--references == 0) {
                delete myInterface;
                myInterface = (I*)0;
            }
        }

        /// Returns the Implementation
        /*! The Implementation will be created here on-demand.
         */
        inline I * GetImplementation(void);

        /// Registers an Implementation
        /*! This function must be called for each Implementation (probably from its constructor).
            Checks the current and the new priority and selects the higher one to use.
         */
        void _RegisterImplementation(AutonBase<I> * impl);

        /// Head of the list of Implementation heplers
        /*! This is a chained list, used in the case if new Implementation is selected by name.
            \see \ref AutonBase::Chain() - on how the chain is built
            \see \ref AutonInterface::_ForceImplementation() - on how the selection by name works
         */
        AutonBase<I> * head;

        /// The current Implementation helper
        /*! Points to the helper class of the currently elected Implementation. This is an important
            part of the algorythm: the smart pointer \ref Auton can use it immediately without
            thinking, and therefore it can be as fast as possible.
         */
        AutonBase<I> * implementation;

        /// This is the current Implementation
        /*! It can be NULL if it has not been instantiated yet, or has already been deleted.
         */
        I * myInterface;

        /// Reference counter for the Interface
        /*! \see AutonInterface::Drop()
            \see AutonInterface::Use()
         */
        int references;

        /// The Implementation Name
        /*! \note   It is stored only for debug purposes.
         */
        const char * myTypeName;

        /// The full class name
        /*! \note   It is stored only for debug purposes.
         */
        char myName[60];

        /// Mutex for this pointer
        /*! It will lock all the operation to guarantee thread-safety for \ref Auton.
         */
        Glib::Mutex myMutex;

        static AutonInterface<I> myStaticImplementation;
    };

    /// Generic, Implementation-independent base of class \ref AutonHandler
    /*! This is only an accessor class, to satisfy the access rights to members.
     */
    template <class I>
    class AutonBase
    {
        friend class AutonInterface<I>;

        virtual I * Create(void) =0;

     protected:
        inline AutonBase(const char * name, int prio):
            typeName(name),
            priority(prio)
        {
            Chain(AutonInterface<I>::myStaticImplementation.head);
            AutonInterface<I>::myStaticImplementation._RegisterImplementation(this);
        }

        virtual ~AutonBase()
        {
        }

        /// Chains itself to the list of Implementations
        /*! Note that the order within the list is not important (and parctically unknown), so
            it is put into its head, because this is the fastest way and runs in constant time.
         */
        inline void Chain(AutonBase<I> *& head)
        {
            next = head;
            head = this;
        }

        /// The next element of the Implementation List
        AutonBase<I> * next;

        /// The name of the Implementation
        const char * typeName;

        /// The priority of the Implementation
        int priority;
    };

    /// Makes connection between the Interface and the Implementation
    template <class C, class I>
    class AutonHandler: public AutonBase<I>
    {
     public:
        AutonHandler(const char * name, int prio):
            AutonBase<I>(name, prio)
        {
            SYS_DEBUG_MEMBER(DM_AUTON);
            SYS_DEBUG(DL_INFO1, "Name: '" << name << "', priority: " << prio);
        }

     private:
        SYS_DEFINE_CLASS_NAME("_AutonPrivate::AutonHandler<C,I>");

        /// Returns a new instance of the Implementation
        virtual I * Create(void)
        {
            SYS_DEBUG_MEMBER(DM_AUTON);
            return new C;
        }

        static AutonHandler<C, I> myStaticImplementation;
    };

    /// Selects the proper implementation for the given inetrface
    /*! This function is called from each instance of AutonHandler with corresponding Interface
        type. The most suitable one (with highest priority) will be selected here.
     */
    template <class I>
    void AutonInterface<I>::_RegisterImplementation(AutonBase<I> * impl)
    {
        if (!implementation) {
            // The very first one:
            SetImplementation(impl);
        } else if (impl->priority > implementation->priority) {
            // Has higher priority:
            SetImplementation(impl);
        } else if (impl->priority == implementation->priority) {
            // This means a race condition: the class initialized earlier wins.
            // Probably this is error (or at least warning), should be handled somehow: TODO
        }
    }

    /// Selects another Implementation by name
    /*! This function can be called from the running code at any time (using the macro
        \ref AUTON_FORCE) to re-select the Implementation. Note that the first call of the
        corresponding \ref Auton smart pointer creates the Implementation, and the existing
        \ref Auton pointers keep the current Implementation while the last one is closed.
        After this, creating the first \ref Auton smart pointer instantiates the newly selected
        Implementation.<br>
        Nothing happens at all if the current Implementation is selected again.
        \note   The running time is linear, according to the number of implementations. The
                running time of the smart pointer \ref Auton is not affected.
     */
    template <class I>
    bool AutonInterface<I>::_ForceImplementation(const char * typeName)
    {
        for (AutonBase<I> * current = myStaticImplementation.head; current; current = current->next) {
            if (!strcmp(typeName, current->typeName)) {
                // Found:
                myStaticImplementation.SetImplementation(current);
                return true;
            }
        }

        // Not found:
        return false;
    }
} // namespace _AutonPrivate

// ****** Public macros and classes:

/// Defines the class 'I' as an Interface
/*! This macro must be used once for each Interface class. Any class can be used as an interface,
    no restrictions and no precautions.
    \note   This macro generates definitions.
 */
#define AUTON_INTERFACE(I) \
    template<> _AutonPrivate::AutonInterface<I> _AutonPrivate::AutonInterface<I>::myStaticImplementation(#I)

/// Implements the given Interface 'I' using Implementation 'C'
/*! Any number of implementation can be given for an Interface, the one with highest
    priority will be used.
    \param  C       This is the implementation of the Interface I
    \param  I       The Interface to be implemented
    \param  prio    This is the priority. It is an integer value, higher value means
                    higher priority. Do not use implementations with the same priority
                    because the result may be undefined in this case. No other restrictions.<br>
                    Note that the default priority used in macro \ref AUTON_IMPLEMENT is zero.
                    It is recommended to use large steps (e.g. 100 or 1000) for the non-default
                    implementations to be able to add smaller and larger values later.
    \note   This macro generates definitions.
 */
#define AUTON_IMPLEMENT_PRIO(C, I, prio) \
    template<> _AutonPrivate::AutonHandler<C,I> MAKE_REFERENCED _AutonPrivate::AutonHandler<C,I>::myStaticImplementation(#C, prio)

/// Implements 'I' using class 'C' with default (0) priority
/*! \see ::AUTON_IMPLEMENT_PRIO for details
    \note   This macro generates definitions.
 */
#define AUTON_IMPLEMENT(C, I) \
    AUTON_IMPLEMENT_PRIO(C, I, 0)

/// Overrides the elected Implementation by the given name
/*! Calling this macro selects another Implementation by the given name.<br>
    \param  I       The Interface class to be implemented
    \param  name    The name (in 'const char *' type) of the class to be used as the
                    Implementation for Interface 'I'
    \note   This macro generates executable code.
            The function \ref _AutonPrivate::AutonInterface::_ForceImplementation is called, see it
            for more details.
 */
#define AUTON_FORCE(I, name) \
    ::_AutonPrivate::AutonInterface<I>::myStaticImplementation._ForceImplementation(name)

/// Declares friend(s) necessary for Interface classes
/*! Adding this definition to a class allows to be all members private, making higher reliability of the source.<br>
    It is not necessary to use (just recommended), but at least the constructor must be public without it.
    \param      C       The Implementation class
    \param      I       The Interface class
 */
#define MAKE_AUTON_FRIEND(C, I) \
    friend class ::_AutonPrivate::AutonHandler<C, I>

/// Singleton smart pointer for the given Interface
/*! \see \ref UseAuton for more details.
    \see \ref AutoImp for a non-singleton variant of this class
    \param  I       This is the Interface to be used. The Implementation used is selected by the priority
                    out of ones defined by the macro ::AUTON_IMPLEMENT or ::AUTON_IMPLEMENT_PRIO.
                    Also see ::AUTON_FORCE on how to override the priorities.
 */
template <class I>
class Auton
{
 public:
    /*! The constructor does nothing, just increments the usage counter of the Interface.
     */
    inline Auton(void)
    {
        _AutonPrivate::AutonInterface<I>::myStaticImplementation.Use();
    }

    /*! The destructor decrements the usage counter and deletes the Implementation if it is
        not beig used.
     */
    inline ~Auton()
    {
        _AutonPrivate::AutonInterface<I>::myStaticImplementation.Drop();
    }

    /// Checks if the Implementation is instantiated or not
    static inline bool IsValid(void)
    {
        return _AutonPrivate::AutonInterface<I>::myStaticImplementation.IsValid();
    }

    /*! The pointer operator creates the Implementation on-demand.
     */
    inline I * operator->()
    {
        return _AutonPrivate::AutonInterface<I>::myStaticImplementation.GetImplementation();
    }

    /*! The reference operator creates the Implementation on-demand.
     */
    inline I & operator*()
    {
        return *_AutonPrivate::AutonInterface<I>::myStaticImplementation.GetImplementation();
    }
};

/// Smart pointer for the given Interface
/*! \param  I       This is the Interface to be used. The Implementation used is selected by the priority
                    out of ones defined by the macro ::AUTON_IMPLEMENT or ::AUTON_IMPLEMENT_PRIO.
                    Also see ::AUTON_FORCE on how to override the priorities.Instantiating such a class creates a new instance of the Implementation of the Interface 'I'.<br>

    This is not a singleton, any number of instance can be created and there is no reference counter:
    all such smart pointer creates and deletes of its own Implementation.
    \see \ref UseAutoImp for more details.
    \see \ref Auton for a singleton variant of this class
 */
template <class I>
class AutoImp
{
 public:
    inline AutoImp(void)
    {
        myself = _AutonPrivate::AutonInterface<I>::CreateImplementation();
    }

    inline ~AutoImp()
    {
        delete myself;
    }

    inline I * operator->()
    {
        return myself;
    }

    inline I & operator*()
    {
        return *myself;
    }

 private:
    I * myself;
};

namespace _AutonPrivate
{
    /// Creates the Implementation on demand
    /*! If the target has already been instantiated, nothing happens.
        \note It is useful for class \ref Auton
     */
    template <class I>
    inline I * AutonInterface<I>::GetImplementation(void)
    {
        SYS_DEBUG_MEMBER(DM_AUTON);
        if (myInterface) return myInterface; // It is already created, nothing to do
        Glib::Mutex::Lock _l(myMutex);
        if (myInterface) return myInterface; // Somebody else has just created it
        myInterface = CreateImplementation();
        return myInterface;
    }
}

#endif /* __AUTON_H__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
