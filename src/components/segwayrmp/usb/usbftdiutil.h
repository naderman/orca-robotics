#ifndef USBFTDIUTIL_H
#define USBFTDIUTIL_H

#include <string>
#include <ftd2xx.h>

namespace segwayrmp
{

    // Returns a human-readable status string
    std::string ftStatusToString( FT_STATUS status );

}

#endif
