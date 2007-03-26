#ifndef RMPEXCEPTION_H
#define RMPEXCEPTION_H

#include <exception>

namespace segwayrmp {

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

}

#endif
