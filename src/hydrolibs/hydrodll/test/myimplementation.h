#ifndef HYDRO_DLLTEST_MY_IMPLEMENTATION_H
#define HYDRO_DLLTEST_MY_IMPLEMENTATION_H

#include "interface.h"
#include <hydroportability/sharedlib.h>

namespace dlltest {

class SOEXPORT MyImplementation : public Interface
{
public:
    MyImplementation( int num ) : number_(num) {};

    virtual int number() const;

private:
    int number_;
};

// Helper class to instantiate drivers
class SOEXPORT MyFactory : public Factory
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
     SOEXPORT dlltest::Factory *createFactory();
}

#endif
