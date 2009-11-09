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

#ifndef MHI_PA10_COMMAND_RESPONSE_H
#define MHI_PA10_COMMAND_RESPONSE_H

#include "mhidefs.h"
#include "unitconverter.h"
#include "message.h"
#include "model.h"

#include <vector>
#include <stdint.h>
#include <hydrointerfaces/mhipa10.h>

namespace mhipa10arc 
{


class MotionCommandResponse
{
	// Servo received data format
	struct ServoDataMsg
	{
		uint16_t	status;		// Status
		int32_t	ang;			// Joint angle
		int16_t	spd;			// Joint velocity
		int16_t	trq;			// Joint torque
	} __attribute__((__packed__));

	struct ArmDataMsg
	{
		ServoDataMsg	servoData[7];
		uint16_t		status;
	} __attribute__((__packed__));


	// Entire packet - message plus header
	struct MessageFormat
	{
		Message::Header hdr;
		ArmDataMsg data;
	} __attribute__((__packed__));

public:
	MotionCommandResponse( MessagePtr& msg );
	virtual ~MotionCommandResponse() {}

	// Is there a warning or error condition from the communication CPU
	bool isCommWarn() const;
	bool isCommFault() const;
	
	// Is there a warning or error condition from one of the servo CPUs
	bool isServoWarn( unsigned int servoNo ) const;
	bool isServoFault( unsigned int servoNo ) const;
	
	// Gets all current arm status messages as a string.
	std::string getStatusAsString() const;

	// Prints out in human-readable form
	std::string toString() const;

	// Allow direct access to the raw data
	const MessagePtr& rawMessage() const { return msg_; }
	
	// Return the data
	const hydrointerfaces::MitsubishiPA10::ManipulatorData& data() const {return armdata_;}

private:
	// Convert to a more friendly format
	void convertMessageToData();
	
	hydrointerfaces::MitsubishiPA10::ManipulatorData armdata_;
	MessagePtr msg_;
};

typedef std::auto_ptr<MotionCommandResponse> MotionCommandResponsePtr;

}

#endif
