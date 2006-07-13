#include <orcaice/application.h>

#include "component.h"

//
// Build the component into a stand-alone application
//
int main(int argc, char * argv[])
{
    featuremaploader::Component component;
    orcaice::Application app( component );
    return app.main(argc, argv);
}
