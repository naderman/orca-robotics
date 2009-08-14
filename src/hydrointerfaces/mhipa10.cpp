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

#include <sstream>
#include "mhipa10.h"

namespace hydrointerfaces {

const float MitsubishiPA10::maxRatedMotorTorque[7] = {4.64, 4.64, 2.0, 2.0, 0.29, 0.29, 0.29};	// N*m

std::string MitsubishiPA10::ServoData::toString() const
{
    std::stringstream ss;

    /*ss << "  seconds:  " << seconds  << std::endl;
    ss << "  useconds: " << useconds << std::endl;
    ss << "  x:        " << x        << std::endl;
    ss << "  y:        " << y        << std::endl;
    ss << "  roll:     " << roll     << std::endl;
    ss << "  pitch:    " << pitch    << std::endl;
    ss << "  yaw:      " << yaw      << std::endl;
    ss << "  vx:       " << vx       << std::endl;
    ss << "  droll:    " << droll    << std::endl;
    ss << "  dpitch:   " << dpitch   << std::endl;
    ss << "  dyaw:     " << dyaw     << std::endl;
    ss << "  mainvolt: " << mainvolt << std::endl;
    ss << "  uivolt:   " << uivolt   << std::endl;*/

    return ss.str();
}

std::string MitsubishiPA10::ManipulatorData::toString() const
{
	std::stringstream ss;

	return ss.str();
}

std::string MitsubishiPA10::ServoCmd::toString() const
{
    std::stringstream ss;
    //ss << "[ "<<vx<<"m/s, "<<w*180.0/Pi<<"deg/s ]";

    return ss.str();
}

std::string MitsubishiPA10::ManipulatorCommand::toString() const
{
    std::stringstream ss;
    //ss << "[ "<<vx<<"m/s, "<<w*180.0/Pi<<"deg/s ]";

    return ss.str();
}

MitsubishiPA10::ManipulatorCommand MitsubishiPA10::createBrakeONCmd()
{
	ManipulatorCommand armcmd;
	for (unsigned int n=0; n < 7; ++n)
	{
		armcmd.servoCmds.push_back( MitsubishiPA10::ServoCmd(true, true, false, 0.0) );
	}
	return armcmd;
}

MitsubishiPA10::ManipulatorCommand MitsubishiPA10::createBrakeOFFNoMotionCmd()
{
	ManipulatorCommand armcmd;
	for (unsigned int n=0; n < 7; ++n)
	{
		armcmd.servoCmds.push_back( MitsubishiPA10::ServoCmd(true, false, false, 0.0) );
	}
	return armcmd;
}

} // namespace
