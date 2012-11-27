#include "Auton.h"
#include <Debug/Debug.h>

AUTON_INTERFACE(AutonLock);

/// The default locker for class \ref Auton
/*! This is an empty class, does not lock anything.<br>
    If you want to make Auton thread-safe, just implement another class with higher
    priority to do lock/unlock whatever you want.
 */
class AutonDefaultLock: public AutonLock
{
 public:
    AutonDefaultLock()
    {
    }

    ~AutonDefaultLock()
    {
    }
};

AUTON_IMPLEMENT(AutonDefaultLock, AutonLock);

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
