#include <iostream>
#include <orcalogfactory/defaultautologgerfactory.h>
#include <orcalogfactory/defaultreplayerfactory.h>

using namespace std;
using namespace orcalogfactory;

//
// Dynamic lib loading means that nightly checks don't find unresolved symbols.
// This test fixes that.
//
int main()
{
    DefaultAutoLoggerFactory l;
    DefaultReplayerFactory r;

    cout<<"test PASSED" << endl;
    return 0;
}
