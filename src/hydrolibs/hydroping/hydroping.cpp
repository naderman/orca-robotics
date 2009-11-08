#include "hydroping.h"
#include <iostream>
#include <hydroutil/sysutils.h>
#include <string.h>
#include <stdlib.h>

using namespace std;

namespace hydroping {

double pingLatencyMs( const std::string &host,
                      bool               runWithSudo,
                      const std::string &pathToHydroPing )
{
    std::string cmd;
    if ( runWithSudo )
        cmd = "sudo ";
    cmd += pathToHydroPing + " ";
    cmd += host + " ";
    cmd += "--parseable";

    std::string failReason, output;

    const bool success = hydroutil::executeSystemCommand( cmd, failReason, &output );
    if ( !success )
    {
        throw gbxutilacfr::Exception( ERROR_INFO, failReason );
    }

    const char *errorPrefix = "ERR";
    if ( !strncmp(output.c_str(),errorPrefix,sizeof(char)*strlen(errorPrefix)) )
    {
        throw PingException( ERROR_INFO, output );
    }

    return atof( output.c_str() );
}

}

