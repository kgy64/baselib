#include "memory-test.h"
#include <iostream>

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

AUTON_IMPLEMENT_PRIO(M3, I1, 2);
AUTON_IMPLEMENT_PRIO(X2, I2, 2);

AUTON_IMPLEMENT_PRIO(M2, I1, 3); // The highest
AUTON_IMPLEMENT_PRIO(X3, I2, 3);

AUTON_IMPLEMENT_PRIO(M1, I1, 1);
AUTON_IMPLEMENT_PRIO(X1, I2, 1);

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
