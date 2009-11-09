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

 
#ifndef HYDRO_MHI_PA10_ARCNET_H
#define HYDRO_MHI_PA10_ARCNET_H

#include <hydrointerfaces/mhipa10.h>

#include "arcnetio.h"
#include "response.h"
#include "unitconverter.h"
#include "model.h"

					
namespace mhipa10arc
{

/*
  	An implementation of HwHandler that knows how to talk the PA10
	protocol.  Requires a hardware interface implementation to be
	passed to the constructor.

 	@author J.D. Yamokoski
*/
class Driver : public hydrointerfaces::MitsubishiPA10
{
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
	// Clears any servo alarms
	void clearAlarms();
	
	virtual void checkCommand( ManipulatorCommand& cmd );
	
	// Reading to and writing from the PA10 is a coupled.
	MessagePtr doMessageExchange(MessagePtr sendmsg);
	
	// Most-recently-received data stored as a smart pointer
	MotionCommandResponsePtr lastCmdResponse_;
	
	// Arcnet interface on which to talk to PA10
	ArcnetIO iodev_;
	
	// Flag to remember if new data has come in since the previous read attempt
	bool isNewData_;
	
	// Flag to remember if the start motion control command was sent
	bool isMotionControlStarted_;
	
	// Local storage of the manipulator description
	ManipulatorConfig config_;
	
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
