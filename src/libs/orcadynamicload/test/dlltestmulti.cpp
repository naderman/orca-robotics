#include <orcadynamicload/dynamicload.h>
#include <iostream>
#include "interface.h"

using namespace std;

int main()
{
    orcadynamicload::DynamicallyLoadedLibrary* lib;

    cout<<"loading library ...";
    lib = new orcadynamicload::DynamicallyLoadedLibrary( "libOrcaDllTestMulti.so" );
    cout<<"ok"<<endl;

    cout<<"loading factory ...";
    dlltest::Factory* factory;
    factory = orcadynamicload::dynamicallyLoadClass<dlltest::Factory,FactoryMakerFunc>
                        ( *lib, "createFactory" );
    cout<<"ok"<<endl;

    cout<<"creating an instance of type 2 ...";
    {
        dlltest::Interface* interface;
        int type = 2;
        interface = factory->create( type );
        if ( interface->number()!=type ) {
            cout<<"failed: wrong number; expect="<<type<<", got="<<interface->number()<<endl;
            return EXIT_FAILURE;
        }   
    }
    cout<<"ok"<<endl;

    cout<<"creating an instance of type 3 ...";
    {
        dlltest::Interface* interface;
        int type = 3;
        interface = factory->create( type );
        if ( interface->number()!=type ) {
            cout<<"failed: wrong number; expect="<<type<<", got="<<interface->number()<<endl;
            return EXIT_FAILURE;
        }   
    }
    cout<<"ok"<<endl;

    cout<<"creating an instance of unsupported type ...";
    {
        dlltest::Interface* interface;
        int type = 999;
        try {
            interface = factory->create( type );
            cout<<"failed: this type should not be supported"<<endl;
            return EXIT_FAILURE;
        }
        catch ( const std::string& ) {}
    }
    cout<<"ok"<<endl;

    return 0;
}
