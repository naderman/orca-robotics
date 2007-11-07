#include "buttonelement.h"
#include <iostream>
#include <orcaice/orcaice.h>
#include <orcaqgui/exceptions.h>

using namespace std;

namespace orcaqgui2d {

ButtonElement::ButtonElement( const orcaice::Context  &context,
                              const std::string       &proxyString )
    : context_(context),
      proxyString_(proxyString)
{
    // try to connect at once
    connect();
}

void
ButtonElement::connect()
{
    orcaice::connectToInterfaceWithString( context_, buttonPrx_, proxyString_ );
}

QStringList
ButtonElement::contextMenu()
{
    QStringList s;
    s<<"Press";
    return s;
}

void
ButtonElement::execute( int action )
{
    switch ( action )
    {
    case 0:
        press();
        break;
    default:
        stringstream ss;
        ss << "execute(): What the hell? bad action: " << action;
        throw orcaqgui::Exception( ss.str() );
        break;
    }
}

void
ButtonElement::press()
{
    const uint MAX_TRIES = 2;

    for ( uint i=0; i < MAX_TRIES; i++ )
    {
        try {
            buttonPrx_->press();
            break;
        }
        catch ( Ice::Exception &e )
        {
            stringstream ss;
            ss << "ButtonElement: failed to press(): " << e;
            context_.tracer()->error( ss.str() );
        }
    }
}

}

