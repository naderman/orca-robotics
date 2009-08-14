#include "multiimplementation.h"

namespace dlltest {

int TwoImplementation::number() const
{
    return number_;
}

int ThreeImplementation::number() const
{
    return number_;
}

} // namespace

extern "C" {
    dlltest::Factory *createFactory()
    { return new dlltest::MyFactory; }
}

