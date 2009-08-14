#include <hydrodll/dynamicload.h>
#include <iostream>
#include <cstdlib>
#include "interface.h"

using namespace std;

int main()
{
    hydrodll::DynamicallyLoadedLibrary* lib;

    cout<<"loading library ...";
#ifndef WIN32
    lib = new hydrodll::DynamicallyLoadedLibrary( "libHydroDllTestOne.so" );
#else
    lib = new hydrodll::DynamicallyLoadedLibrary( "HydroDllTestOne.dll" );
#endif
    cout<<"ok"<<endl;

    cout<<"loading factory ...";
    dlltest::Factory* factory;
    factory = hydrodll::dynamicallyLoadClass<dlltest::Factory,FactoryMakerFunc>
                        ( *lib, "createFactory" );
    cout<<"ok"<<endl;

    cout<<"getting access to the interface ...";
    dlltest::Interface* iface;
    iface = factory->create( 2 );
    if ( iface->number()!=2 ) {
        cout<<"failed: wrong number; expect=2, got="<<iface->number()<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    return 0;
}
