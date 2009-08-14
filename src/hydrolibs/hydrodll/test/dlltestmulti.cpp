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
    lib = new hydrodll::DynamicallyLoadedLibrary( "libHydroDllTestMulti.so" );
#else
    lib = new hydrodll::DynamicallyLoadedLibrary( "HydroDllTestMulti.dll" );
#endif

    cout<<"ok"<<endl;

    cout<<"loading factory ...";
    dlltest::Factory* factory;
    factory = hydrodll::dynamicallyLoadClass<dlltest::Factory,FactoryMakerFunc>
                        ( *lib, "createFactory" );
    cout<<"ok"<<endl;

    cout<<"creating an instance of type 2 ...";
    {
        dlltest::Interface* iface;
        int type = 2;
        iface = factory->create( type );
        if ( iface->number()!=type ) {
            cout<<"failed: wrong number; expect="<<type<<", got="<<iface->number()<<endl;
            return EXIT_FAILURE;
        }   
    }
    cout<<"ok"<<endl;

    cout<<"creating an instance of type 3 ...";
    {
        dlltest::Interface* iface;
        int type = 3;
        iface = factory->create( type );
        if ( iface->number()!=type ) {
            cout<<"failed: wrong number; expect="<<type<<", got="<<iface->number()<<endl;
            return EXIT_FAILURE;
        }   
    }
    cout<<"ok"<<endl;

    cout<<"creating an instance of unsupported type ...";
    {
        dlltest::Interface* iface;
        int type = 999;
        try {
            iface = factory->create( type );
            cout<<"failed: this type should not be supported"<<endl;
            return EXIT_FAILURE;
        }
        catch ( const std::string& ) {}
    }
    cout<<"ok"<<endl;

    return 0;
}
