#include <Debug/Debug.h>
#include <iostream>
#include "memory-test.h"

int main(int argc, char ** argv)
{
 g_thread_init(NULL);

 std::cout << "* Started ------------------ " << std::endl;
 {
    Auton<I1> interface;
    std::cout << "* Interface created." << std::endl;
    std::cout << "* " << (interface.IsValid() ? "valid" : "not valid") << std::endl;
    std::cout << interface->n() << std::endl;
    std::cout << "* " << (interface.IsValid() ? "valid" : "not valid") << std::endl;
    std::cout << interface->n() << std::endl;
    std::cout << interface->n() << std::endl;
    std::cout << "* " << (interface.IsValid() ? "valid" : "not valid") << std::endl;
    std::cout << "* Destroying:" << std::endl;
    interface.Destroy();
    std::cout << "* Destroyed." << std::endl;
    std::cout << "* " << (interface.IsValid() ? "valid" : "not valid") << std::endl;
    std::cout << interface->n() << std::endl;
    std::cout << interface->n() << std::endl;
 }
 std::cout << "* Forced ------------------ " << std::endl;
 std::cout << "Force to 'M1': " << (AUTON_FORCE(I1, "M1") ? "succeeded" : "failed") << std::endl;
 std::cout << "Force to 'M3': " << (AUTON_FORCE(I1, "M3") ? "succeeded" : "failed") << std::endl;
 {
    Auton<I1> interface;
    std::cout << "* Interface created." << std::endl;
    std::cout << "* " << (interface.IsValid() ? "valid" : "not valid") << std::endl;
    std::cout << interface->n() << std::endl;
    std::cout << "* " << (interface.IsValid() ? "valid" : "not valid") << std::endl;
    std::cout << interface->n() << std::endl;
    std::cout << interface->n() << std::endl;
    std::cout << "* " << (interface.IsValid() ? "valid" : "not valid") << std::endl;
    std::cout << "* Destroying:" << std::endl;
    interface.Destroy();
    std::cout << "* Destroyed." << std::endl;
    std::cout << "* " << (interface.IsValid() ? "valid" : "not valid") << std::endl;
    std::cout << interface->n() << std::endl;
    std::cout << "Force to 'M2': " << (AUTON_FORCE(I1, "M2") ? "succeeded" : "failed") << std::endl;
    std::cout << "Force to 'M1': " << (AUTON_FORCE(I1, "M1") ? "succeeded" : "failed") << std::endl;
    std::cout << interface->n() << std::endl;
    std::cout << "Force to 'ABC': " << (AUTON_FORCE(I1, "ABC") ? "succeeded" : "failed") << std::endl;
    std::cout << interface->n() << std::endl;
 }
 std::cout << "* Exited -------------------- " << std::endl;
 return 0;
}

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
