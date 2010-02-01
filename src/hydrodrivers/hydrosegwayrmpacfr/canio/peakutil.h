#ifndef SEGWAYRMPACFR_PEAKUTIL_H
#define SEGWAYRMPACFR_PEAKUTIL_H

#include <libpcan.h>
#include <string>

namespace segwayrmpacfr {

    std::string versionString( HANDLE &portHandle );
    std::string errorInfo( DWORD retVal );
    // Call this only after an error occurs (checks errno)
    std::string errorInfoWithErrno( DWORD retVal );
    std::string peakStatusToString( DWORD status );

    std::string messageTypeToString( BYTE messageType );
    std::string toString( const TPCANMsg &msg );
    inline std::string toString( const TPCANRdMsg &msg ) { return toString(msg.Msg); }

}

#endif
