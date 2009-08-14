/*************************************************************************
	
	date		: August 2008
	copyright	: (C) 2005 J.D. Yamokoski
	email	: yamokosk+gnu at gmail dot com

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public License as
 published by the Free Software Foundation; either version 2.1 of the License,
 or (at your option) any later version. The text of the GNU Lesser General
 Public License is included with this library in the file LICENSE.TXT.
 
 This library is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the file LICENSE.TXT for
 more details.
 
 *************************************************************************/

#include <iostream>
#include <sstream>
#include <fstream>
#include <assert.h>
#include <IceUtil/IceUtil.h>    // for time functions
#include <hydroiceutil/timeutils.h>
#include <gbxutilacfr/mathdefs.h>
#include <gbxutilacfr/exceptions.h>

#include "driver.h"
#include "mhidefs.h"
#include "message.h"

#define REPORT_WARN( msg )								\
	{												\
		std::stringstream ss;							\
		ss << "Driver::" << __FUNCTION__ << "(): " << msg;	\
		context_.tracer().warning( ss.str() );				\
	}
																			
#define REPORT_ERR_AND_THROW( msg )								\
	{														\
		std::stringstream ss;									\
		ss << "Driver::" << __FUNCTION__ << "(): " << msg;			\
		context_.tracer().error( ss.str() );						\
		throw hydrointerfaces::MitsubishiPA10::Exception( ss.str() );	\
	}
								
namespace mhipa10arc
{

Driver::Driver( const hydrointerfaces::MitsubishiPA10::ManipulatorConfig& config, const hydroutil::Context &context ) : 
	iodev_(),
	isNewData_(false),
	isMotionControlStarted_(false),
	config_(config),
	context_(context)
{    

}

Driver::~Driver()
{
	if ( !isMotionEnabled() ) 
	{
		// Should never leave shutdown up to the destructor!
		stopMotionControl();
	}
	iodev_.disable();
}

void Driver::enable()
{
	iodev_.disable();

	// Intitialize arcnet device
	try {
		iodev_.enable();
	}
	catch ( std::exception &e )
	{
		REPORT_ERR_AND_THROW( "Failed to enable. Looks like PA10 is not ON or arcnet subsystem is down. Symptom is: " << std::endl << e.what() )
	}
	   
	// Read initial state of the arm, but to do this we need to send a dummy command
   	bool inWarn=false, inFault=false; std::string statusmsg;
	try {
		clearAlarms();
		startMotionControl();
		write( MitsubishiPA10::createBrakeONCmd() );
		stopMotionControl();
		getStatus(statusmsg, inWarn, inFault);
	} catch ( std::exception &e ) {
		REPORT_ERR_AND_THROW( "Write failed while trying to get arm initial conditions. Reason: " << e.what() )
	}
	
	if (inFault)
	{
		REPORT_WARN( "Hardware fault occured during startup. Trying to repair." )
		
		// Try to clear alarms one more time. Don't worry about exceptions being thrown here. If none were
		// thrown above, we should not get any here.. at least I hope.
		clearAlarms();
		startMotionControl();
		write( MitsubishiPA10::createBrakeONCmd() );
		stopMotionControl();
		inFault = false;		
		getStatus(statusmsg, inWarn, inFault);
		
		if (inFault) {
			REPORT_ERR_AND_THROW( "Failed to clear hardware fault: " << statusmsg << std::endl )
		} else {
			REPORT_WARN( "Repaired hardware fault." )
		}
	}
	
	if (inWarn)
	{
		REPORT_WARN( "Hardware warning during startup: " << lastCmdResponse_->getStatusAsString() << std::endl )
	}
}

bool Driver::read( ManipulatorData& data )
{
	if ( lastCmdResponse_.get() == NULL ) {
		REPORT_WARN( "No command responses have been received yet. Has the driver been enabled?" );
		return false;
	}

	bool dataChanged = isNewData_;	
	data = lastCmdResponse_->data();
	isNewData_ = false;
	return dataChanged;
}

void Driver::write( const ManipulatorCommand& command )
{
	// Is the PA10 in a controllable state?
	if ( !isMotionEnabled() ) 
	{
		REPORT_WARN( "Motion has not been enabled. Ignoring command." );
		return;
	}
	
	// Make local copy of the command.. may need to make some changes
	ManipulatorCommand localCopy = command;
	
	// Check if we're exceeding any limits and modify as needed.
	checkCommand( localCopy );
    
	// Send the command to the PA10 and get its response
	MotionCommandFactory cmdMessageFactory( localCopy );
	MessagePtr cmdmsg = cmdMessageFactory.createMessage();
	MessagePtr recvmsg = doMessageExchange( cmdmsg );
	lastCmdResponse_.reset( new MotionCommandResponse( recvmsg ) );
	isNewData_ = true;
}

void Driver::checkCommand( ManipulatorCommand& cmd )
{
	if ( lastCmdResponse_.get() != NULL ) {
		ManipulatorData data = lastCmdResponse_->data();
		
		if ( data.servoData.size() != cmd.servoCmds.size() )
		{
			REPORT_ERR_AND_THROW( "Number of valid joints in servo data does not match the number in the most recent command. Shutting down." )
		}
		
		for (unsigned int n=0; n < data.servoData.size(); ++n)
		{
			if ( cmd.servoCmds[n].isTorqueCmd ) // Torque command
			{
				/* Difficult to guard against joint limit overrun. Simply check if previous angle is in the vicinity
				   of the joint limit and then if the new command will take it any closer, just brake the joint. */
				if ( ((data.servoData[n].angle > 0.95 * config_.links[n].MaxJointAngle) && (cmd.servoCmds[n].cmdValue > 0)) || 
					((data.servoData[n].angle < 0.95 * config_.links[n].MinJointAngle) && (cmd.servoCmds[n].cmdValue < 0)) )
				{
					cmd.servoCmds[n].brakeON = true;
					REPORT_WARN( "Servo " << config_.links[n].Name << " approaching joint limit. Braking joint." )
				}
			} else { // Velocity command
				/* First compute how much the joint will move in 6ms using the new velocity command. Add 
				   this delta to the previously recorded joint angle and see if it will exceed the joint limit. */
			 	float delta = cmd.servoCmds[n].cmdValue * 0.006; // 6 ms is nominally max time between commands
			 	if ( (data.servoData[n].angle + delta > config_.links[n].MaxJointAngle) ||
			 	     (data.servoData[n].angle + delta < config_.links[n].MinJointAngle) )
				{
					// Could probably do a better job than just setting to zero, but this will suffice for now
					cmd.servoCmds[n].cmdValue = 0;
					REPORT_WARN( "Servo " << config_.links[n].Name << " approaching joint limit. Setting command to zero." )
				}
			}
		}
	}
	
	for (unsigned int n=0; n < cmd.servoCmds.size(); ++n)
	{
		// Convert link torque to motor torque
		if ( cmd.servoCmds[n].isTorqueCmd ) cmd.servoCmds[n].cmdValue /= config_.links[n].GearRatio;
	}
}

void Driver::getStatus( std::string &status, bool &isWarn, bool &isFault )
{
	isWarn = false;
	isFault = false;
	std::stringstream ss;
			
	if ( lastCmdResponse_->isCommWarn() )
	{
		//ss << lastCmdResponse_->getCPUStatus();
		isWarn = true;
	}	
	
	if ( lastCmdResponse_->isCommFault() )
	{
		//ss << lastCmdResponse_->getCPUStatus();
		isFault = true;
	}
	
	unsigned int index = 0;
	FOREACH_OPERATIONAL_AXIS(axisno)
		if ( lastCmdResponse_->isServoWarn( axisno ) )
		{
			//ss << "Servo " << config_.links[index].name " in warning: " << lastCmdResponse_->getServoWarningMsg();
			isWarn = true;
		}
		
		if ( lastCmdResponse_->isServoWarn( axisno ) )
		{
			//ss << "Servo " << config_.links[index].name " in fault: " << lastCmdResponse_->getServoFaultMsg();
			isFault = true;
		}
		index++;
	ENDEACH_OPERATIONAL_AXIS
	
	status = ss.str();
}
	                                  
void Driver::startMotionControl()
{
	StartControlFactory startControlMessageFact;
	MessagePtr msg = startControlMessageFact.createMessage();
	doMessageExchange(msg);
	isMotionControlStarted_ = true;
}

void Driver::stopMotionControl()
{
	StopControlFactory stopControlMessageFact;
	MessagePtr msg = stopControlMessageFact.createMessage();
	doMessageExchange(msg);
	isMotionControlStarted_ = false;
}
	
void Driver::clearAlarms()
{
	// Send an 'A' packet which clears any servo alarms or errors and prepares each
	// servo for the control communication cycle.
	FOREACH_OPERATIONAL_AXIS(axisno)
		AlarmClearFactory alarmClearFactory( axisno+1 );
		MessagePtr clearMsg = alarmClearFactory.createMessage();
		doMessageExchange( clearMsg );
	ENDEACH_OPERATIONAL_AXIS
}

// Reading to and writing from the PA10 is a coupled.
MessagePtr Driver::doMessageExchange(MessagePtr sendmsg)
{
	// Send the command to the PA10
	try {
		iodev_.writeMessage(sendmsg);
	} catch ( const hydrointerfaces::MitsubishiPA10::Exception &e ) {
        REPORT_ERR_AND_THROW( "Failed to write message:" << std::endl << sendmsg->toString() << std::endl << "Reason: " << e.what() )
	}
    
	MessagePtr recvmsg;
	try {
		recvmsg = iodev_.readMessage();
	} catch ( const hydrointerfaces::MitsubishiPA10::Exception &e ) {
		REPORT_ERR_AND_THROW( "Failed to read message." << std::endl << "Reason: " << e.what() )
	}

	// Check that sequence numbers match
	if ( sendmsg->frameNumber() !=  recvmsg->frameNumber() )
	{
		REPORT_ERR_AND_THROW( "Frame count out of sync. Sent " << (int)sendmsg->frameNumber() << " but received " << (int)recvmsg->frameNumber() )
	}
	
	return recvmsg;
}

hydrointerfaces::MitsubishiPA10* MitsubishiPA106CFactory::createDriver( const hydrointerfaces::MitsubishiPA10::ManipulatorConfig& config, const hydroutil::Context& context ) const
{
	return new Driver(config, context);
}

} // namespace


extern "C" {
    
hydrointerfaces::MitsubishiPA10Factory *createDriverFactory()
{ 
	return new mhipa10arc::MitsubishiPA106CFactory; 
}

}
 








