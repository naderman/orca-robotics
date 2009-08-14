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

// Boost stuff
#include <boost/bind.hpp>
#include <boost/function.hpp>

// Local includes
#include "driver.h"

#define REPORT_WARN( msg )			std::stringstream ss;							\
								ss << "Driver::" << __FUNCTION__ << "(): " << msg;	\
								context_.tracer().warning( ss.str() );
																			
#define REPORT_ERR_AND_THROW( msg )	std::stringstream ss;									\
								ss << "Driver::" << __FUNCTION__ << "(): " << msg;			\
								context_.tracer().error( ss.str() );						\
								throw hydrointerfaces::MitsubishiPA10::Exception( ss.str() );
namespace mhipa10arc
{

ColumnVector Driver::oldCommand_(6);
Robot* Driver::model_(NULL);


Driver::Driver( const hydrointerfaces::MitsubishiPA10::ManipulatorConfig& config, const hydroutil::Context &context ) : 
	isNewData_(false),
	isMotionControlStarted_(false),
	config_(config),
	oldState_(12),
	context_(context)
{    
	oldState_ = 0.0;
	Driver::oldCommand_ = 0.0;
}

Driver::~Driver()
{
	if ( !isMotionEnabled() ) 
	{
		// Should never leave shutdown up to the destructor!
		stopMotionControl();
	}
	
	if ( Driver::model_ ) delete Driver::model_;
}

void Driver::enable()
{
	// Using config data, create a robot model
	Matrix params(6,23); // Storage of params
	params = 0.0;
	
	/* Column Variable Description
		1      σ        joint type (revolute=0, prismatic=1)
		2      θ        Denavit-Hartenberg parameter
		3      d        Denavit-Hartenberg parameter
		4      a        Denavit-Hartenberg parameter
		5      α        Denavit-Hartenberg parameter
		6      θmin     minimum value of joint variable
		7      θmax     maximum value of joint variable
		8      θ of f   joint oﬀset
		9      m        mass of the link
		10     cx       center of mass along axis x
		11     cy       center of mass along axis y
		12     cz       center of mass along axis z
		13     Ixx      element xx of the inertia tensor matrix
		14     Ixy      element xy of the inertia tensor matrix
		15     Ixz      element xz of the inertia tensor matrix
		16     Iyy      element y y of the inertia tensor matrix
		17     Iyz      element y z of the inertia tensor matrix
		18     Iz z     element z z of the inertia tensor matrix
		19     Im       motor rotor inertia
		20     Gr       motor gear ratio
		21     B        motor viscous friction coeﬃcient
		22     Cf       motor Coulomb friction coeﬃcient
		23     immobile ﬂag for the kinematics and inverse kinematics
	*/
	for (unsigned int dof = 1; dof <= 6; ++dof)
	{
		params(dof, 2) = config_.links[dof-1].JointAngle;
		params(dof, 3) = config_.links[dof-1].JointOffset;
		params(dof, 4) = config_.links[dof-1].LinkLength;
		params(dof, 5) = config_.links[dof-1].TwistAngle;
		params(dof, 6) = config_.links[dof-1].MinJointAngle;
		params(dof, 7) = config_.links[dof-1].MaxJointAngle;
		params(dof, 8) = config_.links[dof-1].JointAngleOffset;
		params(dof, 9) = config_.links[dof-1].LinkMass;
		params(dof, 10) = config_.links[dof-1].CenterOfMass[0];
		params(dof, 11) = config_.links[dof-1].CenterOfMass[1];
		params(dof, 12) = config_.links[dof-1].CenterOfMass[2];
		params(dof, 13) = config_.links[dof-1].Inertia[0];
		params(dof, 14) = config_.links[dof-1].Inertia[1];
		params(dof, 15) = config_.links[dof-1].Inertia[2];
		params(dof, 16) = config_.links[dof-1].Inertia[3];
		params(dof, 17) = config_.links[dof-1].Inertia[4];
		params(dof, 18) = config_.links[dof-1].Inertia[5];
		params(dof, 19) = config_.links[dof-1].MotorInertia;
		params(dof, 20) = config_.links[dof-1].GearRatio;
		params(dof, 21) = config_.links[dof-1].ViscousFriction;
		params(dof, 22) = config_.links[dof-1].CoulombFriction;
		params(dof, 23) = config_.links[dof-1].Immobile;
	}

	Driver::model_ = new Robot(params);
}

bool Driver::read( ManipulatorData& data )
{
	bool dataChanged = isNewData_;	
	
	data.servoData.clear();
	for (unsigned int n=0; n < 6; ++n)
	{
		ServoData servo;
		servo.angle = oldState_(n+1);
		servo.speed = oldState_(n+7);
		servo.torque = Driver::oldCommand_(n+1);
		
		data.servoData.push_back( servo );
	}
	isNewData_ = false;
	return dataChanged;
}


void Driver::checkCommand( ManipulatorCommand& cmd )
{
	for (unsigned int n=0; n < cmd.servoCmds.size(); ++n)
	{
		// Convert link torque to motor torque
		if ( !cmd.servoCmds[n].isTorqueCmd ) 
		{
			REPORT_ERR_AND_THROW( "Currently only torque commands accepted." )
		}
	}
}

void Driver::write( const ManipulatorCommand& command )
{
	// Is the PA10 in a controllable state?
	if ( !isMotionEnabled() ) 
	{
		REPORT_WARN( "Motion has not been enabled. Ignoring command." );
		return;
	}
	
	// Check if we're exceeding any limits
	//checkCommand( command );
    
	// Right we are only handling torque commands.. needs to be expanded to include velocity commands as well
	for (unsigned int i=0; i<6; i++)
	{
		Driver::oldCommand_(i+1) = command.servoCmds[i].cmdValue;
	}
			
	// Integrate xdot
	RowVector tout; Matrix xout; int nok=0,nbad=0;
	Real t0 = 0.0;		// Starting time
	Real tf = 2e-3;	// Time to integrate to
	Real eps = 1e-4; 	// Controls accuracy
	Real h = 1e-5;		// Starting step size
	Real hmin = 1e-12;	// Lower allowable bound on step size
	Real dtsav = 1e-5;	// Time increments to save solution
	
	odeint(calc_xdot, oldState_, t0, tf, eps, h, hmin, nok, nbad, tout, xout, dtsav);
	
	isNewData_ = true;
}

void Driver::getStatus( std::string &status, bool &isWarn, bool &isFault )
{
	isWarn = false;
	isFault = false;
	status = "";
}
	                                  
void Driver::startMotionControl()
{
	isMotionControlStarted_ = true;
}

void Driver::stopMotionControl()
{
	isMotionControlStarted_ = false;
}

ReturnMatrix Driver::calc_xdot(Real t, const Matrix& xin)
{
	ColumnVector xdot(12);
	xdot.rows(1,6) = xin.rows(7,12);
	xdot.rows(7,12) = Driver::model_->acceleration(xin.rows(1,6), xin.rows(7,12), Driver::oldCommand_);
	xdot.Release(); return xdot;
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
 








