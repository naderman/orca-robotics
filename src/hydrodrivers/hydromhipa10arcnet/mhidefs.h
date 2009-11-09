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
 
#ifndef HYDRO_MHI_PA10_CONFIGURATION_H
#define HYDRO_MHI_PA10_CONFIGURATION_H

#include <string>

/*
 * author yamokosk
 *
 * Definition of PA10 status bits. The creation of the enum and char arrays may look
 * a bit odd. Its using a little known and rarely used C-preprocessor technique called X-macros.
 * See http://en.wikipedia.org/wiki/C_preprocessor#X-Macros for more information on them.
 */
#define SERVO_STATUS	FIELD(0x0001, PositiveAngleLimit, "Positive angle limit reached.")	\
						FIELD(0x0002, NegativeAngleLimit, "Negative angle limit reached.")	\
						FIELD(0x4000, ModeUncontrollable, "Servo is uncontrollable.")		\
						FIELD(0x8000, PowerOff, "Servo power OFF.")

#define SERVO_ALARMS	FIELD(0x0010, AlarmSharedMemory, "Shared memory anomaly. (A)")										\
						FIELD(0x0020, AlarmEEPROM, "EEPROM anomaly. (A)")													\
						FIELD(0x0050, AlarmCPU, "CPU anomaly. (A)")															\
						FIELD(0x0060, AlarmVelocityDeviation, "Velocity deviation anomaly. (A)")							\
						FIELD(0x0070, AlarmResolverDeviation, "Resolver deviation anomaly. (A)")							\
						FIELD(0x0080, AlarmPositionLimit, "Position limit exceeded. (A)")									\
						FIELD(0x0090, AlarmMotorTorque, "Motor torque anomaly. (A)")										\
						FIELD(0x00A0, AlarmIPM, "IPM anomaly. (A)")															\
						FIELD(0x00B0, AlarmBrakeShrtCircuit, "Brake short circuit detected. (A)")							\
						FIELD(0x00C0, AlarmMtrSideResolverShrtCircuit, "Motor side resolver short circuit detected. (A)")	\
						FIELD(0x00D0, AlarmGrSideResolverShrtCircuit, "Gear side resolver short circuit detected. (A)")		\
						FIELD(0x00E0, AlarmOverCurrent, "Current limit exceeded. (A)")										\
						FIELD(0x00F0, AlarmOverVelocity, "Velocity limit exceeded. (A)")									\
						FIELD(0x0100, AlarmUnknown, "Unknown CPU anomaly. (A)")												\
						FIELD(0x0110, AlarmEStopSwOn, "E-stop switch closed. (A)")											\
						FIELD(0x0120, AlarmDeadmanSwOff, "Dead-man switch opened. (A)")										\
						FIELD(0x0130, AlarmLimitSwOn, "Limit switch closed. (A)")											\
						FIELD(0x0FF0, AlarmCommTimeout, "Communication timeout. (A)")

#define COMM_STATUS		FIELD(0x0001, DeadmanSwOn, "Dead-man switch closed.")					\
						FIELD(0x0002, PowerTempAnomaly, "Power source temperature anomaly.")	\
						FIELD(0x0004, 100VOutputOn, "100V power output ON.")					\
						FIELD(0x0008, EStopSwOff, "E-stop switch open.")						\
						FIELD(0x1000, TeachingSwOn, "Teaching switch closed.")					\
						FIELD(0x2000, LimitSwOff, "Limit switch open.")							\
						FIELD(0x4000, ModeUncontrollable, "PA10 is uncontrollable.")

#define COMM_ALARMS		FIELD(0x0020, AlarmEEPROM, "EEPROM anomaly. (A)")										\
						FIELD(0x0030, AlarmArcnetInit, "ARCNET initialization error. (A)")						\
						FIELD(0x0040, AlarmCPU, "CPU anomaly. (A)")												\
						FIELD(0x0050, AlarmCommTimeout, "Communication with upper controller timed out. (A)")	\
						FIELD(0x0060, AlarmPowerTemp, "Power source temperature anomaly. (A)")					\
						FIELD(0x0070, Alarm100VOutput, "100V output anomaly. (A)")								\
						FIELD(0x0100, AlarmUnknown, "Unknown anomaly occured. (A)")								\
						FIELD(0x0110, AlarmEStopSwOn, "E-stop switch closed. (A)")								\
						FIELD(0x0120, AlarmDeadmanSwOff, "Dead-man switch opened. (A)")							\
						FIELD(0x0130, AlarmLimitSwOn, "Limit switch closed. (A)")

namespace mhipa10arc
{

enum ServoFlags
{
	ServoFlagPlaceholder=-1,
#define	FIELD(flag, ID, desc) Servo##ID = flag,
	SERVO_STATUS
	SERVO_ALARMS
#undef FIELD
};

static const unsigned int NumServoFlags = 22;

static const int ServoBitMasks[] = 
{
#define	FIELD(flag, ID, desc) flag,
	SERVO_STATUS
	SERVO_ALARMS
#undef FIELD
};

static const char* ServoFlagDesc[] = 
{
#define	FIELD(flag, ID, desc) desc,
	SERVO_STATUS
	SERVO_ALARMS
#undef FIELD
};

enum CommCPUFlags
{
#define	FIELD(flag, ID, desc) Comm##ID = flag,
	COMM_STATUS
	COMM_ALARMS
#undef FIELD
};

static const unsigned int NumCommCPUFlags = 17;

static const int CommBitMasks[] = 
{
#define	FIELD(flag, ID, desc) flag,
	COMM_STATUS
	COMM_ALARMS
#undef FIELD
};

static const char* CommFlagDesc[] = 
{
#define	FIELD(flag, ID, desc) desc,
	COMM_STATUS
	COMM_ALARMS
#undef FIELD
};

// Conversion constants
static const float 	SPEED_TO_COUNT = 5000.0;
static const float	TORQUE_TO_COUNT = 4096.0;
static const float 	COUNT_TO_SPEED = 2e-4;
static const float	COUNT_TO_TORQUE = 0.244140625e-3;
static const double COUNT_TO_DEGREE = 0.439453125e-3;


} // End namespace

#endif
