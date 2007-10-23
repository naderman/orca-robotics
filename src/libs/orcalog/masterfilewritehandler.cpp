#include "masterfilewritehandler.h"
#include <iostream>

using namespace std;

namespace orcalog {

MasterFileWriteHandler::MasterFileWriteHandler( const LogWriterInfo &logWriterInfo )
    : logWriterInfo_(logWriterInfo),
      id_(-1),
      numItemsLogged_(0)
{
    // record our slave ID
    id_ = logWriterInfo_.masterFileWriter.addLog( logWriterInfo_.filename,
                                                  logWriterInfo_.interfaceType,
                                                  logWriterInfo_.format,
                                                  logWriterInfo_.comment );
}

void
MasterFileWriteHandler::writeReferenceToMasterFile()
{
    assert( id_ >= 0 );
//     if ( id_ < 0 ) {
//         context_.tracer()->warning( interfaceTag_+"LogWriter: received object when not registered. Ignoring.");
//         return;
//     }

    //
    // append master file, increment data counter
    //
    logWriterInfo_.masterFileWriter.notifyOfLogfileAddition( id_, numItemsLogged_ );
    numItemsLogged_++;

    // In debug mode, let the user know that something's going on.
    if ( numItemsLogged_ % 50 == 1 ) {
        ostringstream ss;
        ss << logWriterInfo_.comment << ": logged " << numItemsLogged_ << " data objects.";
        // logWriterInfo_.context.tracer()->debug( ss.str() );
        cout << ss.str() << endl;
    }
}

}

