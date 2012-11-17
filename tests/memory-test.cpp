#include <iostream>
#include "memory-test.h"

AUTON_IMPLEMENT_PRIO(M1, I1, 1);
AUTON_IMPLEMENT_PRIO(X1, I2, 1);

class I1
{
 public:
    virtual ~I1() {}
    virtual int n() = 0;
};

class I2
{
 public:
    virtual ~I2() {}
    virtual int n() = 0;
};

#define MY(name, interface, result) \
    class name: public interface \
    { \
        public: \
        name() \
        { \
            std::cout << #name << " created." << std::endl; \
        } \
        ~name() \
        { \
            std::cout << #name << " deleted." << std::endl; \
        } \
        virtual int n() \
        { \
            return result; \
        } \
    }

MY(X1, I2, 14);
MY(X2, I2, 15);
MY(X3, I2, 16);
MY(X4, I2, 17);
MY(X5, I2, 18);

MY(M1, I1, 3);
MY(M2, I1, 4);
MY(M3, I1, 5);
MY(M4, I1, 6);
MY(M5, I1, 7);

AUTON_INTERFACE(I2);

AUTON_IMPLEMENT_PRIO(M3, I1, 2);
AUTON_IMPLEMENT_PRIO(X2, I2, 2);

AUTON_INTERFACE(I1);

AUTON_IMPLEMENT_PRIO(M2, I1, 3); // The highest
AUTON_IMPLEMENT_PRIO(X3, I2, 3);

int main(int argc, char ** argv)
{
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
