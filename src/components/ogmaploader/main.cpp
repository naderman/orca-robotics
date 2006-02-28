#include <orcaice/application.h>

#include "maploadercomponent.h"

//
// Build the component into a stand-alone application
//
int main(int argc, char * argv[])
{
    MapLoaderComponent component;
    orcaice::Application app( argc, argv, component );
    return app.main(argc, argv);
}
