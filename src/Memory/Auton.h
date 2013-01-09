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

/*!
\page   UseInterfaces   How to define Implementations for \ref Auton and \ref AutoImp
To simplify the Project Design, \a Interfaces can be used having unlimited number of
\a Implementations.<br>
To use the corresponding Implementation, see \ref Auton for a singleton instance, or
\ref AutoImp for a non-singleton instance.<br>
    <br>
    Let's see an example:<br>
    - <b>The Interface:</b><br>
    You can use any kind of class as Interface here, but of course, it is necessary
    to have some virtual functions to act as an Interface:
\code
 class MyInterface
 {
    public:
        virtual ~MyInterface() {}
        virtual int Value() =0;
 }
\endcode
    It is also necessary to define the Interface for the \ref Auton usage:
\code
 AUTON_INTERFACE(MyInterface);
\endcode
    Put this definition in one of the cpp sources. It can be anywhere in the code, it is
    not necessary to be visible for the \ref Auton pointer implementations.<br>
    That's enough, you can use the \ref Auton for this Interface like this:
\code
 Auton<MyInterface> something;
 int i = something->Value();
\endcode
    In this example, the variable \a i will be assigned according to the Implementation. The
    Implementation is not visible in this code, so currently we don't know the result. :-)
    Let's see the following paragraph to see the Implementation.<br>
    <br>
    - <b>The Implementation:</b><br>
    In the another part of the source, you can define at least one Implementation for that
    Interface. The Implementation can be in a different object, it is enough for they to meet
    only at linking time.<br>
    Let's see the exmaple Implementation:
\code
 class MyImplementation: public MyInterface
 {
     virtual int Value() { return 3; }
 }
\endcode
    The Implementation classes are also necessary to be defined for the \ref Auton usage:
\code
 AUTON_IMPLEMENT(MyImplementation, MyInterface);
\endcode
    Put it in one of the cpp files. It also can be in another part of the source, it is not
    necessary to be visible for the \ref Auton pointers nor for the Implementation class.<br>
    If you use this Implementation, you will get \a 3 as result in the above example.<br>
    <br>
    - <b>Define more Implementations:</b><br>
    It is possible to have more Implementations at a time, the \ref Auton will select one of
    them according to their \a priority. To do this, use the following macro:
\code
 AUTON_IMPLEMENT_PRIO(MyOtherTry, MyInterface, 1);
\endcode
    Note that in the previous example, the class \a MyImplementation was defined with
    \a priority=0 as a default used by the macro \ref AUTON_IMPLEMENT. So, the \a priority=1
    here will override the other Implementation.<br>
    <br>
    <b>Warning:</b> Do \a not define more classes with the same priority, because in this
    case the selection between them is defined by the initialization order of the
    corresponding static instances, so it is practically unknown.<br>
    <br>
    - <b>Selecting the Implementation by name:</b><br>
    Let's see the following example (see \ref UseAuton for other details):
\code
 Auton<MyInterface> something;
 int i = something->Value();
 AUTON_FORCE(MyInterface, "MyImplementation");
 int j = something->Value();
\endcode
    Assuming that we have the two Implementations mentioned above, the variable \a i will have
    value from class \a MyOtherTry, while \a j will have \a 3, from class \a MyImplementation.<br>
    See the details in the source documentation.<br>
    <b>Notes:</b>
     - The forced selection itself needs linear time (according to the number of implementations), while
     the running time of the pointers is not affected at all.
     - If the \ref AUTON_FORCE is called before the first construction of the corresponding \ref Auton
     instance, then the original Implementation is not instantiated at all.<br>
    <br>
    - <b>The Internals:</b>
     - <b>Independency</b><br>
      All parts of the code (the Interface, the Implementations, and all the definition-like
      macros) can be put in different parts of the source. They must only meet at linking time.<br>
      <br>
     - <b>Optimization</b><br>
      The code is optimized for speed. The election of the highest priority Implementation is
      done in static initialization time, it does not need running time at all. Except the
      selection by name: doing this (and also the static initialization) needs linear time,
      so practically you can use unlimited number of Implementations. Note that selecting by
      name does not bother the \ref Auton pointer behavior, so it has no speed impact at all.<br>
      <br>
     - <b>How does it work:</b>
      - <b>The Interface side:</b><br>
       The macro \ref AUTON_INTERFACE instantiates a template class \ref _AutonPrivate::AutonInterface
       in the anonymous namespace. This is a very small helper class, having only some static members
       (which are also defined by the macro). See the source documentation for more details.<br>
       Its main job is to keep a pointer to the currently elected Implementation helper class (see below).<br>
       <br>
      - <b>The Implementation side:</b><br>
       The macros \ref AUTON_IMPLEMENT and \ref AUTON_IMPLEMENT_PRIO instantiate a template class
       \ref _AutonPrivate::AutonHandler in the anonymous namespace. This is also a very small helper
       class. However, it has static and non-static members, the access to these instances is not necessary,
       because it registers itself into the corresponding Interface helper class (_AutonPrivate::AutonInterface)
       and accessed through its static functions. The registration phase also elects the highest priority
       one in constant time (see \ref _AutonPrivate::AutonInterface::_RegisterImplementation()) and all this
       is done during static initialization. This procedure guarantees fast execution.<br>
       <br>
      - <b>The Smart Pointers \ref Auton and \ref AutoImp</b><br>
       An instance of such a smart pointer uses only the corresponding Interface helper class (see \ref
       _AutonPrivate::AutonInterface) through its static member functions.<br>
       Its operation is <b>optimized for speed</b>: the instance of the Implementation is accessed by a pointer
       of the helper class directly. It is also done exactly the same way after a name-based forced election.<br>
       In \ref Auton it is <b>reference-counted</b> by the helper class, while \ref AutoImp instantiates one
       Implementation for each instance.<br>
       The name-based <b>forced election</b> takes linear time, according to the number of Implementations.
       The speed of all further operations is not affected at all.

<hr>
Download the source from <b>git.teledigit.eu:/git/lib/base.git</b>, see the file <b>src/Memory/Auton.h</b><br>

\page   UseAuton   Auton: smart pointer for singleton Implementations
\see \ref UseInterfaces first.

If you have defined the \a Interface and \a Implementation(s) as described on the page above, usage of \ref Auton
is very simple:
\code
 int i = Auton<MyInterface>()->Value();
\endcode
This example corresponds the description on the previously mentioned page: the Implementation
having highest priority will be instantiated by the first such call. Being singleton, this instance
will be used for all of such smart pointers in the same executable.
<hr>
Download the source from <b>git.teledigit.eu:/git/lib/base.git</b>, see the file <b>src/Memory/Auton.h</b><br>

\page   UseAutoImp  AutoImp: smart pointer for Implementations
\see \ref UseInterfaces first.

If you have defined the \a Interface and \a Implementation(s) as described on the page above, usage of \ref AutoImp
is very simple:
\code
 AutoImp<MyInterface> myInstance;
 ...
 int i = myInstance->Value();
 ...
\endcode
Note that such a smart pointer instantiates the corresponding Implementation for each instance of it.
<hr>
Download the source from <b>git.teledigit.eu:/git/lib/base.git</b>, see the file <b>src/Memory/Auton.h</b><br>
*/

#endif /* __AUTON_H__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
