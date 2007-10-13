#ifndef ORCA_DLLTEST_MY_IMPLEMENTATION_H
#define ORCA_DLLTEST_MY_IMPLEMENTATION_H

#include "interface.h"

namespace dlltest {

class MyImplementation : public Interface
{
public:
    MyImplementation( int num ) : number_(num) {};

    virtual int number() const;

private:
    int number_;
};

// Helper class to instantiate drivers
class MyFactory : public Factory
{
public:
    virtual Interface* create( int number ) const
    {
        return new MyImplementation( number );
    }
};

} // namespace

// Used for dynamically loading driver
extern "C" {
    dlltest::Factory *createFactory();
}

#endif
