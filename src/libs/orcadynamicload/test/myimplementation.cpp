#include "myimplementation.h"

namespace dlltest {

int MyImplementation::number() const
{
    return number_;
}

} // namespace

extern "C" {
    dlltest::Factory *createFactory()
    { return new dlltest::MyFactory; }
}

