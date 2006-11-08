#include "usbftdiutil.h"
#include <iostream>

using namespace std;


namespace segwayrmp
{

    std::string ftStatusToString( FT_STATUS status )
    {
        switch status
        {
        case FT_OK:
            return "FT_OK";
        case FT_INVALID_HANDLE:
            return "FT_INVALID_HANDLE";
        case FT_DEVICE_NOT_FOUND:
            return "FT_DEVICE_NOT_FOUND";
        case FT_DEVICE_NOT_OPENED:
            return "FT_DEVICE_NOT_OPENED";
        case FT_IO_ERROR:
            return "FT_IO_ERROR";
        case FT_INSUFFICIENT_RESOURCES:
            return "FT_INSUFFICIENT_RESOURCES";
        case FT_INVALID_PARAMETER:
            return "FT_INVALID_PARAMETER";
        case FT_INVALID_BAUD_RATE:
            return "FT_INVALID_BAUD_RATE";
        case FT_DEVICE_NOT_OPENED_FOR_ERASE:
            return "FT_DEVICE_NOT_OPENED_FOR_ERASE";
        case FT_DEVICE_NOT_OPENED_FOR_WRITE:
            return "FT_DEVICE_NOT_OPENED_FOR_WRITE";
        case FT_FAILED_TO_WRITE_DEVICE:
            return "FT_FAILED_TO_WRITE_DEVICE";
        case FT_EEPROM_READ_FAILED:
            return "FT_EEPROM_READ_FAILED";
        case FT_EEPROM_WRITE_FAILED:
            return "FT_EEPROM_WRITE_FAILED";
        case FT_EEPROM_ERASE_FAILED:
            return "FT_EEPROM_ERASE_FAILED";
        case FT_EEPROM_NOT_PRESENT:
            return "FT_EEPROM_NOT_PRESENT";
        case FT_EEPROM_NOT_PROGRAMMED:
            return "FT_EEPROM_NOT_PROGRAMMED";
        case FT_INVALID_ARGS:
            return "FT_INVALID_ARGS";
        case FT_NOT_SUPPORTED:
            return "FT_NOT_SUPPORTED";
        case FT_OTHER_ERROR:
            return "FT_OTHER_ERROR";
        default:
            return "Unknown FT Error";
        }
    }
    
}
