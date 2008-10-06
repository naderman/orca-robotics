#include <iostream>
#include <orcaqgui3dfactory/defaultfactory.h>

using namespace std;

//
// Dynamic lib loading means that nightly checks don't find unresolved symbols.
// This test fixes that.
//
int main()
{
    orcaqgui3d::DefaultFactory factory;

    cout<<"TRACE(defaultfactoryinstantiate.cpp): test PASSED" << endl;
    return 0;
}
