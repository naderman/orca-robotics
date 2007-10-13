#include <orcadynamicload/dynamicload.h>
#include <iostream>
#include "interface.h"

using namespace std;

int main()
{
    orcadynamicload::DynamicallyLoadedLibrary* lib;

    cout<<"loading library ...";
    lib = new orcadynamicload::DynamicallyLoadedLibrary( "libOrcaDllTestOne.so" );
    cout<<"ok"<<endl;

    cout<<"loading factory ...";
    dlltest::Factory* factory;
    factory = orcadynamicload::dynamicallyLoadClass<dlltest::Factory,FactoryMakerFunc>
                        ( *lib, "createFactory" );
    cout<<"ok"<<endl;

    cout<<"getting access to the interface ...";
    dlltest::Interface* interface;
    interface = factory->create( 2 );
    if ( interface->number()!=2 ) {
        cout<<"failed: wrong number; expect=2, got="<<interface->number()<<endl;
        return EXIT_FAILURE;
    }
    cout<<"ok"<<endl;

    return 0;
}
