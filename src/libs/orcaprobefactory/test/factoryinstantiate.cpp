#include <iostream>
#include <orcaprobefactory/orcaprobefactory.h>

using namespace std;

//
// Dynamic lib loading means that nightly checks don't find unresolved symbols.
// This test fixes that.
//
int main()
{
    orcaprobefactory::OrcaProbeFactory factory;

    cout<<"TRACE(defaultfactoryinstantiate.cpp): test PASSED" << endl;
    return 0;
}
