#include "peakutil.h"
#include <iostream>
#include <string.h>
#include <sstream>
#include <gbxutilacfr/exceptions.h>
#include <iomanip>

using namespace std;

namespace segwayrmpacfr {

    std::string versionString( HANDLE &portHandle )
    {
        char textBuf[VERSIONSTRING_LEN];
        DWORD ret = CAN_VersionInfo( portHandle, textBuf );
        if ( ret == 0 )
        {
            return textBuf;
        }
        else
        {
            stringstream ss;
            ss << "CAN_VersionInfo failed (ret="<<ret<<"): "<<strerror(nGetLastError());
            throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
        }    
    }

    std::string errorInfo( DWORD retVal )
    {
        stringstream ss;
        ss << "(ret="<<retVal<<"="<<peakStatusToString(retVal)<<")";
        return ss.str();
    }

    std::string errorInfoWithErrno( DWORD retVal )
    {
        stringstream ss;
        ss << "(ret="<<retVal<<"="<<peakStatusToString(retVal)<<"): "<<strerror(nGetLastError());
        return ss.str();
    }

std::string 
peakStatusToString (DWORD status){
    switch (status){
    case CAN_ERR_OK: 
        return "CAN_ERR_OK";        // no error
    case CAN_ERR_XMTFULL:
        return "CAN_ERR_XMTFULL";   // transmit buffer full
    case CAN_ERR_OVERRUN:
        return "CAN_ERR_OVERRUN";   // overrun in receive buffer
    case CAN_ERR_BUSLIGHT:
        return "CAN_ERR_BUSLIGHT";  // bus error, errorcounter limit reached
    case CAN_ERR_BUSHEAVY:
        return "CAN_ERR_BUSHEAVY";  // bus error, errorcounter limit reached
    case CAN_ERR_BUSOFF:
        return "CAN_ERR_BUSOFF";    // bus error, 'bus off' state entered
    case CAN_ERR_QRCVEMPTY:
        return "CAN_ERR_QRCVEMPTY";  // receive queue is empty
    case CAN_ERR_QOVERRUN:
        return " CAN_ERR_QOVERRUN:"; // receive queue overrun
    case CAN_ERR_QXMTFULL:
        return "CAN_ERR_QXMTFULL";   // transmit queue full 
    case CAN_ERR_REGTEST:
        return "CAN_ERR_REGTEST";    // test of controller registers failed
    case CAN_ERR_NOVXD:
        return "CAN_ERR_NOVXD";       // Win95/98/ME only
    case CAN_ERR_RESOURCE:
        return "CAN_ERR_RESOURCE";      // can't create resource
    case CAN_ERR_ILLPARAMTYPE:
        return "CAN_ERR_ILLPARAMTYPE";  // illegal parameter
    case CAN_ERR_ILLPARAMVAL:         
        return "CAN_ERR_ILLPARAMVAL";   // value out of range
    case CAN_ERRMASK_ILLHANDLE:
        return "CAN_ERRMASK_ILLHANDLE"; // wrong handle, handle error
    default:
        return "CAN status unknown.";
    }
}

std::string messageTypeToString( BYTE messageType )
{
    switch ( messageType )
    {
    case 0x80:
        return "STATUS";
    case 0x02:
        return "EXTENDED";
    case 0x01:
        return "RTR";
    case 0x00:
        return "STANDARD";
    default:
        return "Unknown";
    }
}

namespace {
    std::string toHexString( const BYTE *buf, int bufLen )
    {
        stringstream ss;
        ss << "[ ";
        for ( int i=0; i < bufLen; i++ )
        {
            ss <<hex<<std::setfill('0')<<std::setw(2)<<(int)(buf[i])<<" ";
        }
        ss << "]";
        return ss.str();
    }
}

std::string toString( const TPCANMsg &msg )
{
    stringstream ss;

    ss << std::hex << "ID=0x" << (unsigned int)(msg.ID) << ", MSGTYPE=0x"<<(unsigned int)(msg.MSGTYPE)<<"("<<messageTypeToString(msg.MSGTYPE)<<"), LEN="<<(unsigned int)(msg.LEN)<<", DATA=";
    ss << toHexString(msg.DATA,msg.LEN);

    return ss.str();
}

}

