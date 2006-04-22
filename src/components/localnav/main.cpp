#include <orcaice/application.h>

#include "component.h"

//
// Build the component into a stand-alone application
//
int main(int argc, char * argv[])
{
    localnav::Component component;
    orcaice::Application app( argc, argv, component );
    return app.main(argc, argv);
}
