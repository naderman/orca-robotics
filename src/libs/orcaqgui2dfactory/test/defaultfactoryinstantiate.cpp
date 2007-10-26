#include <iostream>
#include <orcaqgui2dfactory/defaultfactory.h>

using namespace std;

//
// Dynamic lib loading means that nightly checks don't find unresolved symbols.
// This test fixes that.
//
int main()
{
    orcaqgui2d::DefaultFactory factory;

    cout<<"TRACE(defaultfactoryinstantiate.cpp): test PASSED" << endl;
    return 0;
}
