#ifndef ORCA_DLLTEST_LIB_INTERFACE_H
#define ORCA_DLLTEST_LIB_INTERFACE_H

#include <string>

namespace dlltest {

// Abstract interface which will be implemented in the library
class Interface
{
public:
    virtual ~Interface() {};

    virtual int number() const=0;
};

// Helper class to instantiate implementation(s)
class Factory 
{
public:
    virtual ~Factory() {};
    virtual Interface *create( int number ) const=0;
};

} // namespace

// Function for dynamically instantiating drivers.
// A driver must have a function like so:
// extern "C" {
//     dlltest::Factory *createFactory();
// }
typedef dlltest::Factory *FactoryMakerFunc();

#endif
