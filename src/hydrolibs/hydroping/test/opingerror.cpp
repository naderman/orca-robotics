#include <iostream>
#include <oping.h>

using namespace std;

int main()
{
    int retAdd, retRemove;
    pingobj_t *pingObj = ping_construct();

    //
    // Test a host that reolves using 'search acfr.usyd.edu.au' from /etc/resolv.conf
    //
    const char *host1="bamboo";
    cout<<"TRACE(opingerror.cpp): Testing "<<host1 << endl;
    retAdd = ping_host_add( pingObj, host1 );
    if ( retAdd!=0 )
    {
        cout<<"TRACE(opingerror.cpp): retAdd: " << retAdd << ": " << ping_get_error(pingObj) << endl;
    }
    retRemove = ping_host_remove( pingObj, host1 );
    if ( retRemove!=0 )
    {
        cout<<"TRACE(opingerror.cpp): retRemove: " << retRemove << ": " << ping_get_error(pingObj) << endl;
    }



    //
    // Test using the fully-qualified domain-name
    //
    const char *host2="bamboo.acfr.usyd.edu.au";
    cout<<"TRACE(opingerror.cpp): Testing "<<host2<< endl;
    retAdd = ping_host_add( pingObj, host2 );
    if ( retAdd!=0 )
    {
        cout<<"TRACE(opingerror.cpp): retAdd: " << retAdd << ": " << ping_get_error(pingObj) << endl;
    }
    retRemove = ping_host_remove( pingObj, host2 );
    if ( retRemove!=0 )
    {
        cout<<"TRACE(opingerror.cpp): retRemove: " << retRemove << ": " << ping_get_error(pingObj) << endl;
    }

}
