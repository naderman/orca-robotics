#ifndef SEGWAYRMP_RMPIO_H
#define SEGWAYRMP_RMPIO_H

namespace segwayrmp
{

class CanPacket;

//
// Exceptions thrown around in segwayrmp.
//
class RmpException : public std::exception
{
public:

    RmpException(const char *message)
        : message_(message) {}
    RmpException(const std::string &message)
        : message_(message) {}

    virtual ~RmpException() throw() {}

    virtual const char* what() const throw() { return message_.c_str(); }

protected:

    std::string  message_;
};

//!
//! @bried An abstract class for communicating with the RMP.
//!
//! @author Alex Brooks
//!
class RmpIo
{

public: 

    enum RmpIoStatus
    {
        OK              = 0,
        NO_DATA         = 1,
    };

    virtual ~RmpIo() {}

    // Initiate comms with the RMP.
    virtual void enable( int debugLevel )=0;

    // Release all resources.
    virtual void disable()=0;
    
    // Returns OK if copied a packet, NO_DATA if not
    virtual RmpIoStatus readPacket( CanPacket* pkt )=0;
    
    virtual void writePacket( CanPacket* pkt )=0;

private: 


};

}

#endif
