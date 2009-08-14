#ifndef HYDRO_DLLTEST_MULTI_IMPLEMENTATION_H
#define HYDRO_DLLTEST_MULTI_IMPLEMENTATION_H

#include "interface.h"
#include <string>
#include <hydroportability/sharedlib.h>

namespace dlltest {

class SOEXPORT TwoImplementation : public Interface
{
public:
    TwoImplementation() : number_(2) {};
    virtual int number() const;

private:
    int number_;
};

class SOEXPORT ThreeImplementation : public Interface
{
public:
    ThreeImplementation() : number_(3) {};
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
        switch ( number )
        { 
            case 2 :
                return new TwoImplementation;
            case 3 :
                return new ThreeImplementation;
            default :
                throw std::string("Unsupported implementation type");
        }
    }
};

} // namespace

// Used for dynamically loading driver
extern "C" {
    SOEXPORT dlltest::Factory *createFactory();
}

#endif
