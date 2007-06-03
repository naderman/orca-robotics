#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/Bottle.h>
#include <stdio.h>

using namespace yarp::os;

int main() {
    Network::init();
    Bottle bot;
    Port input;
    input.open("/receiver");
    // usually, we create connections externally, but just for this example...
    Network::connect("/sender","/receiver");
    // orca: instead of receiving one object, do it in the loop
    while (1) {
        input.read(bot);
        printf("Got message: %s\n", bot.toString().c_str());
    }
    input.close();
    Network::fini();
    return 0;
}
