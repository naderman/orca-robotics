#include "masterfilewritehandler.h"
#include <iostream>

using namespace std;

namespace orcalog {

MasterFileWriteHandler::MasterFileWriteHandler( const LogWriterInfo &logWriterInfo,
                                                MasterFileWriter    &masterFileWriter )
    : logWriterInfo_(logWriterInfo),
      masterFileWriter_(masterFileWriter),
      id_(-1),
      numItemsLogged_(0)
{
    // record our slave ID
    id_ = masterFileWriter_.addLog( logWriterInfo_.filename,
                                    logWriterInfo_.interfaceType,
                                    logWriterInfo_.format,
                                    logWriterInfo_.comment );
}

void
MasterFileWriteHandler::writeReferenceToMasterFile( const orca::Time &arrivalTime )
{
    assert( id_ >= 0 );

    //
    // append master file, increment data counter
    //
    masterFileWriter_.notifyOfLogfileAddition( id_, numItemsLogged_, arrivalTime );
    numItemsLogged_++;

    // In debug mode, let the user know that something's going on.
    if ( numItemsLogged_ % 50 == 1 ) {
        ostringstream ss;
        ss << logWriterInfo_.comment << ": logged " << numItemsLogged_ << " data objects.";
        // logWriterInfo_.context.tracer().debug( ss.str() );
        cout << ss.str() << endl;
    }
}

}

