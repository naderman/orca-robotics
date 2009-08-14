#include "../response.h"
#include "../message.h"
#include "../arcnetio.h"
#include <iostream>

using namespace mhipa10arc;
using namespace std;

#define REPORT_ERR_AND_THROW( msg )	stringstream ss;												\
									ss << "Test::" << __FUNCTION__ << "(): " << msg;				\
									throw hydrointerfaces::MitsubishiPA10::Exception( ss.str() );

MessagePtr doMessageExchange(ArcnetIO& iodev_, const MessagePtr& sendmsg);

								
int main (void)
{
	ArcnetIO* arcio = NULL;
	int ret = 0;
		
	try {

		arcio = new ArcnetIO;
		arcio->enable();
		
		// Send an 'A' packet which clears any servo alarms or errors and prepares each
		// servo for the control communication cycle.
		for (unsigned int axisno = 0; axisno < 7; ++axisno) 
		{
			if (axisno != 3)
			{
				AlarmClearFactory alarmClearFactory( axisno );
				MessagePtr clearMsg = alarmClearFactory.createMessage();
				doMessageExchange( *arcio, clearMsg );
			}
		}
	
		// Create a bunch of commands
		StartControlFactory startControlMessageFact;
		MessagePtr startmsg = startControlMessageFact.createMessage();
		
		MotionCommandFactory cmdMessageFactory2( hydrointerfaces::MitsubishiPA10::createBrakeOFFNoMotionCmd() );
		MessagePtr brakesOFF = cmdMessageFactory2.createMessage();
		
		MotionCommandFactory cmdMessageFactory1( hydrointerfaces::MitsubishiPA10::createBrakeONCmd() );
    		MessagePtr brakesON = cmdMessageFactory1.createMessage();
    		
    		StopControlFactory stopControlMessageFact;
		MessagePtr stopmsg = stopControlMessageFact.createMessage();
		
		// Start motion control
		doMessageExchange(*arcio, startmsg);
	
		// Send brakes on command
		doMessageExchange( *arcio, brakesON );
		
		// Send zero velocity command
		MessagePtr recvmsg;
		for (unsigned int n=0; n < 100; ++n)
		{
			recvmsg = doMessageExchange( *arcio, brakesOFF );
		}
		
		// Turn brakes back on
		doMessageExchange( *arcio, brakesON );
		
		// Stop motion control
		doMessageExchange(*arcio, stopmsg);
		
		MotionCommandResponse response(recvmsg);
		std::cout << response.toString() << std::endl;

	} catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
		ret = -1;
	} catch (...) {
		std::cout << "Unknown exception occured!" << std::endl;
		ret = -1;
	}
	
	// Shut everything down
	if (arcio) {
		arcio->disable();
		delete arcio;
	}
	return ret;
}


// Reading to and writing from the PA10 is a coupled.
MessagePtr doMessageExchange(ArcnetIO& iodev_, const MessagePtr& sendmsg)
{
	// Send the command to the PA10
	iodev_.writeMessage(sendmsg);
    
    MessagePtr recvmsg;
 	recvmsg = iodev_.readMessage();

	// Check that sequence numbers match
	if ( sendmsg->frameNumber() !=  recvmsg->frameNumber() )
	{
		REPORT_ERR_AND_THROW( "Frame count out of sync. Sent " << (int)sendmsg->frameNumber() << " but received " << (int)recvmsg->frameNumber() )
	}
	
    return recvmsg;
}
