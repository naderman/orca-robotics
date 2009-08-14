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

#ifndef HYDRO_MHI_PA10_ARCNET_MESSAGE_H_
#define HYDRO_MHI_PA10_ARCNET_MESSAGE_H_

#include <sys/types.h>
#include <stdint.h>
#include <string.h>
#include <memory>

#include <hydrointerfaces/mhipa10.h>
#include <IceUtil/IceUtil.h>    // for time functions
#include <hydroiceutil/timeutils.h>

#include "model.h"

namespace mhipa10arc
{

/*
  MHI command messages consist of:
    - Header (6 bytes)
    - Data (250 bytes of which a variable amount is used depending on packet type)

  @author J.D. Yamokoski
*/
class Message
{
public:
	// Always use all 256 bytes
	static const int max_msg_length = 256;
	// Frame counter
	static uint8_t frame_count;

	// Packet header
	struct Header
	{
		uint8_t sender;
		uint8_t receiver;
		uint8_t frameNumberAddr;
		uint8_t junkVal;
		uint8_t frameNumber;
		uint8_t msgType;
	};

	Message(unsigned int length);

	// Returns who create this message
	uint8_t sender() {return msg_[0];}
	// Returns who this message was sent to
	uint8_t receiver() {return msg_[1];}
	// Returns the message frame count
	uint8_t frameNumber() {return msg_[4];}
	// Returns the message type
	uint8_t messageType() {return msg_[5];}
	// Returns the actual message length = header + data
	unsigned int length() {return message_length_;}

	// Allow direct access for memcpy
	uint8_t* msg() { return msg_; }
	const uint8_t *msg() const { return msg_; }

	// Human-readable string for debugging
	std::string toString() const;

	// Get/set timestamp
	void timeStamp();
	IceUtil::Time getTimeStamp() const {return timeStamp_;}
	
private:
	uint8_t msg_[max_msg_length];
	unsigned int message_length_;
	IceUtil::Time timeStamp_;
};

typedef std::auto_ptr<Message> MessagePtr;


//////////////////////////////////////////////////////////////////////////////////////////////

class MessageFactory
{
protected:
	MessagePtr msg_;
	uint8_t senderID_;
	uint8_t receiverID_;

public:
	MessageFactory(uint8_t sender=0xFF, uint8_t receiver=0xFE) :
		senderID_(sender), receiverID_(receiver) {}
	virtual ~MessageFactory() {}

	virtual MessagePtr createMessage();

protected:
	virtual void writeHeader();
	virtual void writeMessage() = 0;
	virtual unsigned int length() = 0;
};


//////////////////////////////////////////////////////////////////////////////////////////////

class MotionCommandFactory : public MessageFactory
{
	static const uint16_t	BRAKE_BIT_MASK = 0x0001;
	static const uint16_t	SERVO_POWER_BIT_MASK = 0x0002;
	static const uint16_t	TORQUE_COMMAND_TYPE_BIT_MASK = 0x0004;
	static const uint16_t	OTHER_AXIS_ALARM_BIT_MASK = 0x0010;

	static const uint16_t	DEADMAN_SW_BIT_MASK = 0x0008;

	// Servo command format
	struct ServoMessage
	{
		uint16_t 	status;		// Status: brake ON/OFF, power ON/OFF, spd/trq command
		int16_t 	trqCmdVal;  // Commanded torque value
		int16_t 	spdCmdVal;	// Commanded joint speed
	} __attribute__((__packed__));

	// Arm command format
	struct ArmMessage
	{
		ServoMessage	servoCmd[7];
		uint16_t		commCPU;
	} __attribute__((__packed__));

	// Entire message - message plus header
	struct MessageFormat
	{
		mhipa10arc::Message::Header hdr;
		ArmMessage msg;
	} __attribute__((__packed__));

public:
	MotionCommandFactory( const hydrointerfaces::MitsubishiPA10::ManipulatorCommand& cmd ) :
		MessageFactory(), cmd_(cmd) {}
	MotionCommandFactory( uint8_t sender, uint8_t receiver, const hydrointerfaces::MitsubishiPA10::ManipulatorCommand& cmd ) :
		MessageFactory(sender, receiver), cmd_(cmd) {}
	virtual ~MotionCommandFactory() {}

protected:
	virtual void writeHeader();
	virtual void writeMessage();
	virtual unsigned int length();
	
	hydrointerfaces::MitsubishiPA10::ManipulatorCommand cmd_;
};


//////////////////////////////////////////////////////////////////////////////////////////////

class AlarmClearFactory : public MessageFactory
{
	// Entire packet - message plus header
	struct MessageFormat
	{
		mhipa10arc::Message::Header hdr;
		uint16_t axisNumber;
	} __attribute__((__packed__));

public:
	AlarmClearFactory(uint16_t axisNumber) :
		MessageFactory(), axisNumber_(axisNumber) {}
	AlarmClearFactory(uint8_t sender, uint8_t receiver, uint16_t axisNumber) :
		MessageFactory(sender, receiver), axisNumber_(axisNumber) {}
	virtual ~AlarmClearFactory() {};

protected:
	virtual void writeHeader();
	virtual void writeMessage();
	virtual unsigned int length();

private:
	uint16_t axisNumber_;
};


//////////////////////////////////////////////////////////////////////////////////////////////

class StartControlFactory : public MessageFactory
{
public:
	StartControlFactory() : MessageFactory() {}
	StartControlFactory(uint8_t sender, uint8_t receiver) : MessageFactory(sender, receiver) {}
	virtual ~StartControlFactory() {};

protected:
	virtual void writeHeader();
	virtual void writeMessage();
	virtual unsigned int length();
};


//////////////////////////////////////////////////////////////////////////////////////////////

class StartBrakeReleaseFactory : public MessageFactory
{
public:
	StartBrakeReleaseFactory() : MessageFactory() {}
	StartBrakeReleaseFactory(uint8_t sender, uint8_t receiver) : MessageFactory(sender, receiver) {}
	virtual ~StartBrakeReleaseFactory() {};

protected:
	virtual void writeHeader();
	virtual void writeMessage();
	virtual unsigned int length();
};


//////////////////////////////////////////////////////////////////////////////////////////////

class StopControlFactory : public MessageFactory
{
public:
	StopControlFactory() : MessageFactory() {}
	StopControlFactory(uint8_t sender, uint8_t receiver) : MessageFactory(sender, receiver) {}
	virtual ~StopControlFactory() {};

protected:
	virtual void writeHeader();
	virtual void writeMessage();
	virtual unsigned int length();
};

} // namespace

#endif
