#include <iostream>
#include "sickcarmendriver.h"

using namespace laser2d;

int main()
{
    std::cout<<"This just exists to test linking -- it doesn't do anything." << std::endl;

    // Force the thing to link, so it's easier to find undefined symbols.
    orcaice::Context *context = NULL;
    laser2d::Driver::Config *config = NULL;
    new SickCarmenDriver( *config, *context );

    return 0;
}
