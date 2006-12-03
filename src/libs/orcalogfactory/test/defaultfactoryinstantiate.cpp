#include <iostream>
#include <orcalogfactory/defaultlogfactory.h>
#include <orcalogfactory/defaultreplayfactory.h>

using namespace std;
using namespace orcalogfactory;

//
// Dynamic lib loading means that nightly checks don't find unresolved symbols.
// This test fixes that.
//
int main()
{
    DefaultLogFactory l;
    DefaultReplayFactory r;

    cout<<"TRACE(defaultfactoryinstantiate.cpp): test PASSED" << endl;
    return 0;
}
