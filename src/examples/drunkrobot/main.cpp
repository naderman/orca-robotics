#include <orcaiceutil/application.h>
#include "drunkrobotcomponent.h"

int main(int argc, char * argv[])
{
  DrunkRobotComponent component;
  orcaiceutil::Application app( argc, argv, component );
  return app.main(argc, argv);
}
