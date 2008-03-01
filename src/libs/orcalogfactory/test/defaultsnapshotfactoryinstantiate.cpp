#include <iostream>
#include <orcalogfactory/defaultsnapshotloggerfactory.h>
#include <orcalogfactory/defaultreplayerfactory.h>

using namespace std;
using namespace orcalogfactory;

//
// Dynamic lib loading means that nightly checks don't find unresolved symbols.
// This test fixes that.
//
int main()
{
    DefaultSnapshotLoggerFactory l;
    DefaultReplayerFactory r;

    cout<<"test PASSED" << endl;
    return 0;
}
