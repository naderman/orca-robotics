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

#include "response.h"
#include "unitconverter.h"

// for htons(), htonl(), ntohl()..
#include <arpa/inet.h>

#include <sstream>
#include <iomanip>

namespace mhipa10arc 
{

using namespace std;

MotionCommandResponse::MotionCommandResponse( MessagePtr& msg ) :
	msg_(msg)
{
	convertMessageToData();
}

bool MotionCommandResponse::isCommWarn() const
{
	MotionCommandResponse::MessageFormat* msg = (MotionCommandResponse::MessageFormat*)msg_->msg();
	uint16_t w = ntohs( msg->data.status );

	return ( w & CommPowerTempAnomaly );
}

bool MotionCommandResponse::isCommFault() const
{
	MotionCommandResponse::MessageFormat* msg = (MotionCommandResponse::MessageFormat*)msg_->msg();
	uint16_t w = ntohs( msg->data.status );

	return ( false
#define	FIELD(flag, ID, desc) || (w & flag)
			COMM_ALARMS
#undef FIELD
			);
}

bool MotionCommandResponse::isServoWarn( unsigned int servoNo ) const
{
	MotionCommandResponse::MessageFormat* msg = (MotionCommandResponse::MessageFormat*)msg_->msg();
	uint16_t w = ntohs( msg->data.servoData[servoNo].status );

	return ( (w & ServoPositiveAngleLimit ) || (w & ServoNegativeAngleLimit ) );
}

bool MotionCommandResponse::isServoFault( unsigned int servoNo ) const
{
	MotionCommandResponse::MessageFormat* msg = (MotionCommandResponse::MessageFormat*)msg_->msg();
	uint16_t w = ntohs( msg->data.servoData[servoNo].status );

	return ( false
#define	FIELD(flag, ID, desc) || (w & flag)
			SERVO_ALARMS
#undef FIELD
			);
}

std::string MotionCommandResponse::getStatusAsString() const
{
	MotionCommandResponse::MessageFormat* msg = (MotionCommandResponse::MessageFormat*)msg_->msg();
	uint16_t w = ntohs(msg->data.status);

	stringstream ss;
	ss << "Comm CPU status: " << std::endl;
	for (unsigned int f=0; f<NumCommCPUFlags; ++f)
		if ( w & CommBitMasks[f] ) ss << CommFlagDesc[f] << std::endl;
	
	FOREACH_OPERATIONAL_AXIS(s)
		w = ntohs(msg->data.servoData[s].status);
		
		ss << std::endl << "Servo #" << s+1 << " status:" << std::endl;
		for (unsigned int f=0; f<NumServoFlags; ++f)
			if ( w & ServoBitMasks[f] ) ss << ServoFlagDesc[f] << std::endl;
	ENDEACH_OPERATIONAL_AXIS
	return ss.str();
}

std::string MotionCommandResponse::toString() const
{
	std::stringstream os;
	MotionCommandResponse::MessageFormat* msg = (MotionCommandResponse::MessageFormat*)msg_->msg();

	// Flip all shorts and longs
	FOREACH_OPERATIONAL_AXIS(s)
		os << "Servo " << s+1 << std::endl;
		os << setw(17) << "status" << dec << setw(10) << ntohs(msg->data.servoData[s].status) << hex << setw(10) << "0x" << ntohs(msg->data.servoData[s].status) << std::endl;
		os << setw(17) << "ang" << dec << setw(10) << armdata_.servoData[s].angle << setw(10) << ntohl(msg->data.servoData[s].ang) << hex << setw(10) << "0x" << ntohl(msg->data.servoData[s].ang) << std::endl;
		os << setw(17) << "spd" << dec << setw(10) << armdata_.servoData[s].speed << setw(10) << ntohs(msg->data.servoData[s].spd) << hex << setw(10) << "0x" << ntohs(msg->data.servoData[s].spd) << std::endl;
		os << setw(17) << "trq" << dec << setw(10) << armdata_.servoData[s].torque << setw(10) << ntohs(msg->data.servoData[s].trq) << hex << setw(10) << "0x" << ntohs(msg->data.servoData[s].trq) << std::endl;
	ENDEACH_OPERATIONAL_AXIS

	os << "CommCPU" << std::endl;
	os << setw(17) << "status" << dec << setw(10) << ntohs(msg->data.status) << hex << setw(10) << "0x" << ntohs(msg->data.status) << endl;

	os << "Arm status" << std::endl;
	os << getStatusAsString() << std::endl;
	
	os << "Raw data" << std::endl;
	os << msg_->toString();
	
	return os.str();
}

void MotionCommandResponse::convertMessageToData()
{
	MotionCommandResponse::MessageFormat* msg = (MotionCommandResponse::MessageFormat*)msg_->msg();

	// Flip all shorts and longs
	FOREACH_OPERATIONAL_AXIS(s)
		hydrointerfaces::MitsubishiPA10::ServoData servo;
		
		// Servo status
		uint16_t w = ntohs(msg->data.servoData[s].status);
		servo.isPowered = !(w & ServoPowerOff);
		servo.isControllable = !(w & ServoModeUncontrollable);
		servo.inWarning = isServoWarn(s);
		servo.inFault = isServoFault(s);
		//armdata_.servoData[s].msg.push_back( ServoFlagDesc[f] );
		
		// Angle
		servo.angle = angleFromCounts( ntohl(msg->data.servoData[s].ang) );
		// Velocity
		servo.speed = speedFromCounts( ntohs(msg->data.servoData[s].spd) );
		// Torque
		servo.torque = torqueFromCounts( s, ntohs(msg->data.servoData[s].trq) );
		
		armdata_.servoData.push_back( servo );
	ENDEACH_OPERATIONAL_AXIS

	uint16_t w = ntohs( msg->data.status );
	armdata_.isControllable = !( w & CommModeUncontrollable ) ;
	armdata_.powerOutputOn = w & Comm100VOutputOn;
	armdata_.nominalPowerSourceTemp = !(w & CommPowerTempAnomaly);
	armdata_.limitSwitchOpen = w & CommLimitSwOff;
	armdata_.estopSwitchOpen = w & CommEStopSwOff;
	armdata_.deadmanSwitchClosed = w & CommDeadmanSwOn;
	armdata_.teachingSwitchOpen = w & CommTeachingSwOn;
	armdata_.inWarning = isCommWarn();
	armdata_.inFault = isCommFault();
	
	armdata_.seconds = msg_->getTimeStamp().toSeconds();
	armdata_.useconds = msg_->getTimeStamp().toMicroSeconds();
}

}

