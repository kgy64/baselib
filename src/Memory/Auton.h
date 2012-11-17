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
        first access to the Interface will create the corresponding Implementation.<br>
        This class has a reference counter and deletes the Implementation if the last
        referenced pointer (\ref Auton) is deleted.
     */
    template <class I>
    class AutonInterface
    {
        friend class AutonBase<I>;
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
            myInterface = 0;
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
        /*! Deletes the Implementation if it was the last usage.
            \see AutonInterface::references
         */
        static inline void Drop(void)
        {
            if (--references == 0) {
                Destroy();
            }
        }

        /// Returns the Implementation
        /*! The Implementation will be created on-demand.
         */
        static inline I * Implementation(void)
        {
            Create();
            return myInterface;
        }

        /// Head of the list of Implementation heplers
        /*! This is a chained list, used in the case of new Implementation is selected by name.
            \see \ref AutonBase::Chain()
            \see \ref AutonInterface::_ForceImplementation()
         */
        static AutonBase<I> * head;

        /// The helper class for the current Implementation
        static AutonBase<I> * implementation;

        /// This is the current Implementation
        /*! It can be NULL if it has not been instantiated yet, or has already been deleted.
         */
        static I * myInterface;

        /// Reference counter for the Interface
        static int references;

        /// Registers an Implementation
        /*! This function must be called for each Implementation (probably from its constructor).
            Checks the current and the new priority and selects the higher one to use.
         */
        static void _SelectImplementation(AutonBase<I> * impl);
    };

    /// Generic, Implementation-independent base of class \ref AutonHandler
    template <class I>
    class AutonBase
    {
        friend class AutonInterface<I>;

        virtual I * Create(void) =0;

     protected:
        AutonBase(const char * name, int prio):
            typeName(name),
            priority(prio)
        {
        }

        virtual ~AutonBase()
        {
        }

        void _SelectImplementation(AutonBase<I> * impl)
        {
            AutonInterface<I>::_SelectImplementation(impl);
        }

        /// Chains itself to the given 'head'
        void Chain(AutonBase<I> *& head)
        {
            next = head;
            head = this;
        }

        AutonBase<I> * next;

        const char * typeName;

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
            AutonBase<I>::_SelectImplementation(this);
        }

     private:
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
        impl->Chain(head);

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
                    because the result is undefined in this case. No other restrictions.
    \note   Some variables are declared in anonymous namespace.
 */
#define AUTON_IMPLEMENT_PRIO(C, I, prio) \
    class C; \
    class I; \
    namespace { _AutonPrivate::AutonHandler<C, I> __my_auton_handler_##C##_##prio(#C, prio); }

/// Implements 'I' using class 'C' with default (0) priority
/*! \see ::AUTON_IMPLEMENT_PRIO for details
 */
#define AUTON_IMPLEMENT(C, I) \
    AUTON_IMPLEMENT_PRIO(C, I, 0)

/// Declares the class 'I' as an Interface
/*! This macro must be used once for each Interface class. Any class can be used as an interface,
    no restrictions and no precautions.
    \note   Some variables are declared in anonymous namespace.
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
    \see    \ref _AutonPrivate::AutonInterface::_ForceImplementation() for other details.
    \param  I       The Interface class to be implemented
    \param  name    The name (in 'const char *' type) of the class to be used as the
                    Implementation for Interface 'I'
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
\page   UseAuton   Using smat pointer \ref Auton
    The class ::Auton can be used to implement one of the given singletons through
    an interface.<br>
    Let's see an example:<br>
    <br>

*/

#endif /* __AUTON_H__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
