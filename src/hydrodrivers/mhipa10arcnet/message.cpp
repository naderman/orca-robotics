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

#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <iomanip>

// for htons(), htonl(), ntohl()..
#include <arpa/inet.h>

// Internal
#include "message.h"
#include "unitconverter.h"


namespace mhipa10arc
{

using namespace std;

uint8_t Message::frame_count = 0x0;

Message::Message(unsigned int length) :
	message_length_(length)
{
	// Increment the frame count. To prevent overflow, we will reset the counter
	// if it gets too high.. maybe a better way to do this?
	if (Message::frame_count == 0x7f) Message::frame_count = 0x00;
	else Message::frame_count++;

	// Initialize msg array to zeros
	memset(msg_,0,sizeof(msg_));
}

std::string Message::toString() const
{
	std::ostringstream os;
	os << "Timestamp: " << timeStamp_.toDateTime() << std::endl;
	os << setw(10) << "sender: " << hex<< setw(4) << "0x" << (int)msg_[0] << std::endl;
	os << setw(10) << "receiver: " << hex<< setw(4) << "0x" << (int)msg_[1] << std::endl;
	os << setw(10) << "dataptr: " << hex<< setw(4) << "0x" << (int)msg_[2] << std::endl;
	os << setw(10) << "junkval: " << hex<< setw(4) << "0x" << (int)msg_[3] << std::endl;
	os << setw(10) << "seqnum: " << hex<< setw(4) << "0x" << (int)msg_[4] << std::endl;
	os << setw(10) << "type: " << msg_[5] << std::endl;
	os << "data (" << dec << message_length_ - sizeof(Header) << " bytes): " << std::endl;
	for (unsigned int n=sizeof(Header); n < message_length_; ++n)
	{
		os << setfill(' ') << " " << hex << uppercase << setprecision(2) << setfill('0') << setw(2) << (int)msg_[n];
	}
	return os.str();
}

void Message::timeStamp()
{
	timeStamp_ = IceUtil::Time::now();
}

//////////////////////////////////////////////////////////////////////////////////////////////

MessagePtr MessageFactory::createMessage()
{
	unsigned int pktLength = this->length();
	msg_.reset( new Message( pktLength ) );
	writeHeader();
	writeMessage();
	return msg_;
}

void MessageFactory::writeHeader()
{
	Message::Header* hdr = (Message::Header*)msg_->msg();
	hdr->sender = senderID_;
	hdr->receiver = receiverID_;
	//hdr->frameNumberAddr = 3;
	hdr->frameNumber = Message::frame_count;
	hdr->junkVal = 0xb7; // No idea why this value is what it is. But seems it needs to be in there.
}


//////////////////////////////////////////////////////////////////////////////////////////////

void MotionCommandFactory::writeHeader()
{
	MessageFactory::writeHeader();

	MotionCommandFactory::MessageFormat* pkt = (MotionCommandFactory::MessageFormat*)msg_->msg();

	// Calculation of data pointer:
	//	total_packet_length - ( num_bytes_in( MotionCmdMsg ) + 2_bytes_for_CommandType_and_DataPointer )
	//	256 - (44 + 2) = 210
	pkt->hdr.frameNumberAddr = 210;
	pkt->hdr.msgType  = 'C';
}

void MotionCommandFactory::writeMessage()
{
	MotionCommandFactory::MessageFormat* pkt = (MotionCommandFactory::MessageFormat*)msg_->msg();

	unsigned int cmdIndex = 0;
	FOREACH_OPERATIONAL_AXIS(axisIndex)
		// Construct servo status from command flags
		uint16_t status = 0x0; int16_t value = 0;
		if (cmd_.servoCmds[cmdIndex].servoON) status ^= MotionCommandFactory::SERVO_POWER_BIT_MASK;
		if (cmd_.servoCmds[cmdIndex].brakeON) status ^= MotionCommandFactory::BRAKE_BIT_MASK;
		if (cmd_.servoCmds[cmdIndex].isTorqueCmd) {
			status ^= MotionCommandFactory::TORQUE_COMMAND_TYPE_BIT_MASK;
			value = mhipa10arc::torqueToCounts(axisIndex, cmd_.servoCmds[cmdIndex].cmdValue);
		} else {
			value = mhipa10arc::speedToCounts(cmd_.servoCmds[cmdIndex].cmdValue);
		}

		// Uncomment to make isolated servo alarms brind down the entire arm
		//status ^= OTHER_AXIS_ALARM_BIT_MASK;

		pkt->msg.servoCmd[axisIndex].status = htons(status);
		pkt->msg.servoCmd[axisIndex].trqCmdVal = htons(value);
		pkt->msg.servoCmd[axisIndex].spdCmdVal = htons(value);
		
		// Increment command index
		cmdIndex++;
	ENDEACH_OPERATIONAL_AXIS

	{
		uint16_t status = 0x0;
		// Uncomment to instruct servo CPU to watch for the deadman switch
		//status ^= DEADMAN_SW_BIT_MASK;
		pkt->msg.commCPU = htons(status);
	}
}

inline
unsigned int MotionCommandFactory::length()
{
	return sizeof(MotionCommandFactory::MessageFormat);
}


//////////////////////////////////////////////////////////////////////////////////////////////

void AlarmClearFactory::writeHeader()
{
	MessageFactory::writeHeader();

	AlarmClearFactory::MessageFormat* pkt = (AlarmClearFactory::MessageFormat*)msg_->msg();

	// Calculation of data pointer:
	//	total_packet_length - ( 2_bytes_alarm_clear_msg + 2_bytes_for_CommandType_and_DataPointer )
	//	256 - 4 = 252
	pkt->hdr.frameNumberAddr = 252;
	pkt->hdr.msgType  = 'A';
}

void AlarmClearFactory::writeMessage()
{
	AlarmClearFactory::MessageFormat* pkt = (AlarmClearFactory::MessageFormat*)msg_->msg();
	pkt->axisNumber = htons(axisNumber_);
}

inline
unsigned int AlarmClearFactory::length()
{
	return sizeof(AlarmClearFactory::MessageFormat);
}


//////////////////////////////////////////////////////////////////////////////////////////////

void StartControlFactory::writeHeader()
{
	MessageFactory::writeHeader();

	mhipa10arc::Message::Header* hdr = (mhipa10arc::Message::Header*)msg_->msg();

	// Calculation of data pointer:
	//	total_packet_length - ( 2_bytes_for_CommandType_and_DataPointer )
	//	256 - 2 = 254
	hdr->frameNumberAddr = 254;
	hdr->msgType  = 'S';
}

void StartControlFactory::writeMessage()
{
	// No message
}

inline
unsigned int StartControlFactory::length()
{
	return sizeof(mhipa10arc::Message::Header);
}


//////////////////////////////////////////////////////////////////////////////////////////////

void StartBrakeReleaseFactory::writeHeader()
{
	MessageFactory::writeHeader();

	mhipa10arc::Message::Header* hdr = (mhipa10arc::Message::Header*)msg_->msg();

	// Calculation of data pointer:
	//	total_packet_length - ( 2_bytes_for_CommandType_and_DataPointer )
	//	256 - 2 = 254
	hdr->frameNumberAddr = 254;
	hdr->msgType  = 'T';
}

void StartBrakeReleaseFactory::writeMessage()
{
	// No message
}

inline
unsigned int StartBrakeReleaseFactory::length()
{
	return sizeof(mhipa10arc::Message::Header);
}


//////////////////////////////////////////////////////////////////////////////////////////////

void StopControlFactory::writeHeader()
{
	MessageFactory::writeHeader();

	mhipa10arc::Message::Header* hdr = (mhipa10arc::Message::Header*)msg_->msg();

	// Calculation of data pointer:
	//	total_packet_length - ( 2_bytes_for_CommandType_and_DataPointer )
	//	256 - 2 = 254
	hdr->frameNumberAddr = 254;
	hdr->msgType  = 'E';
}

void StopControlFactory::writeMessage()
{
	// No message
}

inline
unsigned int StopControlFactory::length()
{
	return sizeof(mhipa10arc::Message::Header);
}

}
