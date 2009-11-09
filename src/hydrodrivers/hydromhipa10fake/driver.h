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
 
#ifndef HYDRO_MHI_PA10_FAKE_H
#define HYDRO_MHI_PA10_FALE_H

#include <hydrointerfaces/mhipa10.h>
#include <newmat/newmat.h>
#include <newmat/newmatio.h>
#include <roboop/robot.h>

namespace mhipa10arc
{

/*
  	A fake PA10 driver. Uses an internal model to generate realistic values in response
  	to user commands.

 	@author J.D. Yamokoski
*/
class Driver : public hydrointerfaces::MitsubishiPA10
{
	typedef std::auto_ptr< ManipulatorData > ManipulatorDataPtr;
	
public:
	Driver( const hydrointerfaces::MitsubishiPA10::ManipulatorConfig& config, const hydroutil::Context& context );
	virtual ~Driver();
	
	// from MitsubishiPA10
	virtual void enable();
	virtual bool read( ManipulatorData& data );
	virtual void write( const ManipulatorCommand& command );
	virtual void getStatus( std::string &status, bool &isWarn, bool &isFault );
	
	// To start and stop motion control
	void startMotionControl();
	void stopMotionControl();

	// Returns whether the PA10 is accepting motion control commands
	bool isMotionEnabled() {return isMotionControlStarted_;}

private:
	virtual void checkCommand( ManipulatorCommand& cmd );
	
	// A static function to compute derivatives from model
	static ReturnMatrix calc_xdot(Real t, const Matrix& xin);

	// Flag to remember if new data has come in since the previous read attempt
	bool isNewData_;
	
	// Flag to remember if the start motion control command was sent
	bool isMotionControlStarted_;
	
	// Local storage of the manipulator description
	ManipulatorConfig config_;
	
	// Last state information
	ColumnVector oldState_;
	static ColumnVector oldCommand_;
	
	// Model
	static Robot* model_;
	
	hydroutil::Context context_;
};

class MitsubishiPA106CFactory : public hydrointerfaces::MitsubishiPA10Factory
{
public:
	virtual ~MitsubishiPA106CFactory() {};
	//! Instantiates a driver and returns a pointer to it.
	virtual hydrointerfaces::MitsubishiPA10* createDriver( const hydrointerfaces::MitsubishiPA10::ManipulatorConfig& config, const hydroutil::Context& context ) const;
};


} // namespace

#endif
