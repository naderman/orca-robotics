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

#ifndef MITSUBISHIPA10_
#define MITSUBISHIPA10_

#include <exception>
#include <vector>

#include <hydroutil/context.h>

namespace hydrointerfaces
{

class MitsubishiPA10
{
public:
	static const float maxRatedMotorTorque[7];
	
	struct LinkConfig
	{
		std::string Name;
		float JointAngle;
		float JointOffset;
		float LinkLength;
		float TwistAngle;
		float MinJointAngle;
		float MaxJointAngle;
		float MaxJointVelocity;
		float JointAngleOffset;
		float LinkMass;
		float CenterOfMass[3];
		float Inertia[6]; // Ixx, Ixy, Ixz, Iyy, Iyz, Izz
		float MaxTorque;
		float MotorInertia;
		float GearRatio;
		float ViscousFriction;
		float CoulombFriction;
		bool Immobile;	
	};
	
	struct ManipulatorConfig
	{
		bool AngleInDegree;
		bool IgnoreMotorDynamics;
		std::vector< LinkConfig > links;
	};
	
	//! Servo data
	struct ServoData
	{
		ServoData() :
			isPowered(false),
			isControllable(false),
			inWarning(true),
			inFault(true),
			angle(0.0),
			speed(0.0),
			torque(0.0)
		{ }
		//! Servo power and controllability state
		bool isPowered;
		bool isControllable;
		//! Warning and fault conditions and any servo messages
		bool inWarning;
		bool inFault;
		std::vector< std::string > msg;
		//! Resolver and current data
		double angle;
		float speed;
		float torque;
		//! Converts to a human readable string
		std::string toString() const;
	};

	//! Data structure
	struct ManipulatorData
	{
		ManipulatorData(unsigned int servonum=7) :
			seconds(0),
			useconds(0),
			isControllable(false),
			powerOutputOn(false),
			nominalPowerSourceTemp(false),
			limitSwitchOpen(false),
			estopSwitchOpen(false),
			deadmanSwitchClosed(false),
			teachingSwitchOpen(false),
			inWarning(true),
			inFault(true)
		{
			servoData.resize(servonum);
		}
		//! Timestamp [s]
		int seconds;
		//! Timestamp [us]
		int useconds;
		//! Is arm in a controllable mode?
		bool isControllable;
		//! Misc arm parameters
		bool powerOutputOn;
		bool nominalPowerSourceTemp;
		//! Switch states
		bool limitSwitchOpen;
		bool estopSwitchOpen;
		bool deadmanSwitchClosed;
		bool teachingSwitchOpen;
		//! Warning and fault conditions and any arm messages
		bool inWarning;
		bool inFault;
		std::vector< std::string > msg;
		//! Manipulator mobility
		float mobility;
		//! Servo data
		std::vector<ServoData> servoData;
		//! Converts to a human readable string
		std::string toString() const;
	};

	struct ServoCmd
	{
		ServoCmd(bool servoON=true, bool brakeON=true, bool isTrqCmd=false, float cmdValue=0.0) :
			servoON(servoON), brakeON(brakeON), isTorqueCmd(isTrqCmd), cmdValue(cmdValue) {}
		bool servoON;
		bool brakeON;
		bool isTorqueCmd;
		float cmdValue;
		//! Converts to a human readable string
		std::string toString() const;
	};

	//! Motion command structure
	struct ManipulatorCommand
	{
		//! Timestamp [s]
		int seconds;
		//! Timestamp [us]
		int useconds;
		//! Instruct system in enable or disable motion (regardless of servo commands
		bool enableMotion;
		//! Servo commands
		std::vector<ServoCmd> servoCmds;
		//! Converts to a human readable string
		std::string toString() const;
	};

	//! Exception thrown by implementations of SegwayRmp interface.
	class Exception : public std::exception
	{
	public:
	    //! Constructor with message
	    Exception(const char *message) : message_(message) {}
	    //! Constructor with message
	    Exception(const std::string &message) : message_(message) {}
	    virtual ~Exception() throw() {}
	    //! Returns the message.
	    virtual const char* what() const throw() { return message_.c_str(); }
	protected:
	    std::string  message_;
	};
	
	MitsubishiPA10() {};
	virtual ~MitsubishiPA10() {}

	//! Throws Exception on failure.
	//! It is not an error to call this repeatedly: repeated calls should
	//! re-initialise the hardware.
	//! (eg this will be called if the hardware reports an error condition)
	virtual void enable()=0;

	//! Blocking read.
	//! Returns:
	//!   true:  important change in internal state occured (details can be read with getStatus).
	//!   false: no important change
	virtual bool read( ManipulatorData &data )=0;

	//! Writes motion command.
	virtual void write( const ManipulatorCommand& command )=0;

	//! Get information about the current status of the driver.
	//! the string 'status' is a human-readable description.
	//! Note that there are two ways for faults to be notified:
	//!  - This function tells of hardware faults reported normally by the hardware.
	//!  - Exceptions can be thrown from read/write for non-recoverable faults, such as inability to
	//!    communicate with the hardware.
	virtual void getStatus( std::string &status, bool &isWarn, bool &isFault )=0;

	virtual void startMotionControl() = 0;
	virtual void stopMotionControl() = 0;
	
	// Returns whether the PA10 is accepting motion control commands
	virtual bool isMotionEnabled() = 0;

	virtual void checkCommand( ManipulatorCommand& command ) = 0;
	
	//! Modifies the arguments, thresholding to match what this hardware can actually do.
	/*virtual void applyHardwareLimits( double &maxForwardSpeed,
                                      double &maxReverseSpeed,
                                      double &maxTurnrate,
                                      double &maxLateralAcceleration )=0;*/
	static ManipulatorCommand createBrakeONCmd();
	static ManipulatorCommand createBrakeOFFNoMotionCmd();
};

//! Helper class to instantiate drivers
class MitsubishiPA10Factory
{
public:
	virtual ~MitsubishiPA10Factory() {};
	//! Instantiates a driver and returns a pointer to it.
	virtual MitsubishiPA10* createDriver( const MitsubishiPA10::ManipulatorConfig& config, const hydroutil::Context& context ) const = 0;
};

/*! @} */
} // namespace

// Function for dynamically instantiating drivers.
// A driver must have a function like so:
// extern "C" {
//     hydrointerfaces::SegwayRmpFactory *createMitsubishiPA10DriverFactory();
// }
typedef hydrointerfaces::MitsubishiPA10Factory *MitsubishiPA10DriverFactoryMakerFunc();


#endif /*MITSUBISHIPA10_*/
