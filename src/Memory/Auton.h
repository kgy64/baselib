#ifndef __AUTON_H__
#define __AUTON_H__

#include <string.h>

// Note that in this file 'I' means the Interface class, while 'C' means the Implementation

// Predeclarations:
template <class I> class Auton;

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
        Such an instance will handle the Implementation requests via its function
        AutonInterface::_SelectImplementation()<br>
        This class creates the corresponding Interface implementation on demand: the
        first access to the Interface will instantiate the corresponding Implementation.<br>
        This class has a reference counter and deletes the Implementation if the last
        referenced pointer (\ref Auton) is deleted.
        \note   This class has only static members.
     */
    template <class I>
    class AutonInterface
    {
        /*! The class \ref AutonBase accesses its \ref head and \ref _SelectImplementation()
            members during static initialization time.
         */
        friend class AutonBase<I>;

        /*! The class \ref Auton has right to access its member functions during runtime.
         */
        friend class Auton<I>;

     public:
        /*! Because this helper class is created as a static variable, it is deleted
            only during exit, after the main() function has been returned. The Interface
            instance will be deleted here, if any.
         */
        virtual ~AutonInterface()
        {
            delete myInterface;
        }

        // *** Has already been commented, see below *** //
        static bool _ForceImplementation(const char * typeName);

     private:
        /// Creates the Implementation on demand
        /*! If it has already been instantiated, nothing happens.
         */
        static inline void Create(void)
        {
            if (!myInterface) {
                myInterface = implementation->Create();
            }
        }

        /// Deletes the Implementation if any
        static inline void Destroy(void)
        {
            delete myInterface;
            myInterface = (I*)0;
        }

        /// Checks if the Implementation is instantiated or not
        static inline bool IsValid(void)
        {
            return myInterface;
        }

        /// Increments the reference counter
        /*! \see AutonInterface::references
         */
        static inline void Use(void)
        {
            ++references;
        }

        /// Decrements the reference counter
        /*! Also deletes the Implementation if it was the last usage.
            \see AutonInterface::references
         */
        static inline void Drop(void)
        {
            if (--references == 0) {
                Destroy();
            }
        }

        /// Returns the Implementation
        /*! The Implementation will be created here on-demand.
         */
        static inline I * Implementation(void)
        {
            Create();
            return myInterface;
        }

        /// Head of the list of Implementation heplers
        /*! This is a chained list, used in the case if new Implementation is selected by name.
            \see \ref AutonBase::Chain() - on how the chain is built
            \see \ref AutonInterface::_ForceImplementation() - on how the selection by name works
         */
        static AutonBase<I> * head;

        /// The current Implementation helper
        /*! Points to the helper class of the currently elected Implementation. This is an important
            part of the algorythm: the smart pointer \ref Auton can use it immediately without
            thinking, and therefore it can be as fast as possible.
         */
        static AutonBase<I> * implementation;

        /// This is the current Implementation
        /*! It can be NULL if it has not been instantiated yet, or has already been deleted.
         */
        static I * myInterface;

        /// Reference counter for the Interface
        /*! \see AutonInterface::Drop()
            \see AutonInterface::Use()
         */
        static int references;

        /// Registers an Implementation
        /*! This function must be called for each Implementation (probably from its constructor).
            Checks the current and the new priority and selects the higher one to use.
         */
        static void _SelectImplementation(AutonBase<I> * impl);
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
            Chain(AutonInterface<I>::head);
            AutonInterface<I>::_SelectImplementation(this);
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
        }

     private:
        /// Returns a new instance of the Implementation
        virtual I * Create(void)
        {
            return new C;
        }
    };

    /// Selects the proper implementation for the given inetrface
    /*! This function is called from each instance of AutonHandler with corresponding Interface
        type. The most suitable one (with highest priority) will be selected here.
     */
    template <class I>
    void AutonInterface<I>::_SelectImplementation(AutonBase<I> * impl)
    {
        if (!implementation) {
            implementation = impl; // The very first one
        } else if (impl->priority > implementation->priority) {
            implementation = impl; // Has higher priority
        } else if (impl->priority == implementation->priority) {
            // This means a race condition: the class initialized earlier wins.
            // Probably this is error (or at least warning), should be handled somehow.
        }
    }

    /// Selects another Implementation by name
    /*! This function can be called from the running code at any time (using the macro
        \ref AUTON_FORCE) to re-select the Implementation.<br>
        <br>
        The following actions will be taken:
         - If an Implementation has already been instantiated, it will be deleted.
         - No new Implementation will be instantiated until the next use of the pointer.
         - Nothing happens at all if the current Implementation is selected again.

        \note   The running time is linear, according to the number of implementations. The
                destructor is also can be executed here on demand. The running time of the
                smart pointer \ref Auton is not affected, except the constructor which may
                also be executed on demand there.
     */
    template <class I>
    bool AutonInterface<I>::_ForceImplementation(const char * typeName)
    {
        for (AutonBase<I> * current = head; current; current = current->next) {
            if (!strcmp(typeName, current->typeName)) {
                // Found:
                if (implementation != current) {
                    Destroy();
                    references = 0;
                    implementation = current;
                }
                return true;
            }
        }

        // Not found:
        return false;
    }
} // namespace _AutonPrivate

// ****** Public macros and classes:

/// Implements the given Interface 'I' using class 'C'
/*! Any number of implementation can be given for an Interface, the one with highest
    priority will be used.
    \param  C       This is the implementation of the Interface I
    \param  I       The Interface to be implemented
    \param  prio    This is the priority. It is an integer value, higher value means
                    higher priority. Do not use implementations with the same priority
                    because the result may be undefined in this case. No other restrictions.<br>
                    Note that the default priority used in macro \ref AUTON_IMPLEMENT is zero.
    \note   This macro generates definitions.
 */
#define AUTON_IMPLEMENT_PRIO(C, I, prio) \
    namespace { _AutonPrivate::AutonHandler<C, I> __my_auton_handler_##C##_##prio(#C, prio); }

/// Implements 'I' using class 'C' with default (0) priority
/*! \see ::AUTON_IMPLEMENT_PRIO for details
    \note   This macro generates definitions.
 */
#define AUTON_IMPLEMENT(C, I) \
    AUTON_IMPLEMENT_PRIO(C, I, 0)

/// Defines the class 'I' as an Interface
/*! This macro must be used once for each Interface class. Any class can be used as an interface,
    no restrictions and no precautions.
    \note   This macro generates definitions.
 */
#define AUTON_INTERFACE(I) \
    namespace { template<> _AutonPrivate::AutonInterface<I> __my_auton_interface_##I; } \
    template<> _AutonPrivate::AutonBase<I> * _AutonPrivate::AutonInterface<I>::implementation = 0; \
    template<> _AutonPrivate::AutonBase<I> * _AutonPrivate::AutonInterface<I>::head = 0; \
    template<> I * _AutonPrivate::AutonInterface<I>::myInterface = 0; \
    template<> int _AutonPrivate::AutonInterface<I>::references = 0

/// Overrides the elected Implementation by the given name
/*! Calling this macro removes the current Implementation and selects another one by
    the given name.<br>
    \param  I       The Interface class to be implemented
    \param  name    The name (in 'const char *' type) of the class to be used as the
                    Implementation for Interface 'I'
    \note   This macro generates executable code.
            The function \ref _AutonPrivate::AutonInterface::_ForceImplementation is called, see it
            for more details.
 */
#define AUTON_FORCE(I, name) \
    _AutonPrivate::AutonInterface<I>::_ForceImplementation(name)

/// Smart pointer for the given Interface
/*! \see \ref UseAuton for more details.
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
        _AutonPrivate::AutonInterface<I>::Use();
    }

    /*! The destructor decrements the usage counter and deletes the Implementation if it is
        not beig used.
     */
    inline ~Auton()
    {
        _AutonPrivate::AutonInterface<I>::Drop();
    }

    /// Forces the Implementation to be instantiated
    /*! If it has already been instantiated, nothing happens.
     */
    inline void Create(void)
    {
        _AutonPrivate::AutonInterface<I>::Create();
    }

    /// Forces the Implementation to be deleted, if any
    /*! Note that it deletes the current instance but the next access to any of the
        pointers to this Interface will re-instantiate the Implementation.
     */
    inline void Destroy(void)
    {
        _AutonPrivate::AutonInterface<I>::Destroy();
    }

    /// Checks if the Implementation is instantiated or not
    inline bool IsValid(void)
    {
        return _AutonPrivate::AutonInterface<I>::IsValid();
    }

    /*! The pointer operator creates the Implementation on-demand.
     */
    inline I * operator->()
    {
        return _AutonPrivate::AutonInterface<I>::Implementation();
    }

    /*! The reference operator creates the Implementation on-demand.
     */
    inline I & operator*()
    {
        return *_AutonPrivate::AutonInterface<I>::Implementation();
    }
};

/*!
\page   UseAuton   Auton: smart pointer for singleton implementations
The class ::Auton can be used to implement one of the given singletons through an
    interface.<br>
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
    Let's see the following example:
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
       one in constant time (see \ref _AutonPrivate::AutonInterface::_SelectImplementation()) and all this
       is done during static initialization. This procedure guarantees fast execution.<br>
       <br>
      - <b>The Smart Pointer \ref Auton</b><br>
       An instance of \ref Auton uses only the corresponding Interface helper class (see \ref
       _AutonPrivate::AutonInterface) through its static member functions.<br>
       Its operation is <b>optimized for speed</b>: the instance of the Implementation is accessed by a pointer
       of the helper class directly. It is also done exactly the same way after a name-based forced election.<br>
       It is <b>reference-counted</b> by the helper class.<br>
       The name-based <b>forced election</b> takes linear time, according to the number of Implementations.
       The speed of all further operations is not affected at all.

<hr>
Download the source from <b>git.teledigit.eu:/git/lib/base.git</b>, see the file <b>src/Memory/Auton.h</b><br>
*/

#endif /* __AUTON_H__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
