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

#ifndef HYDRO_MHI_ARCNET_IO_H
#define HYDRO_MHI_ARCNET_IO_H

// External
#include <sys/types.h>
#include <sys/socket.h>
#include <string>

// Internal
#include "message.h"

namespace mhipa10arc
{

class ArcnetIO
{
public:
	// Max number of seconds to wait for a response from the PA10
	const unsigned int ALARM_TIMEOUT;
	const unsigned int SELECT_TIMEOUT;
	
	ArcnetIO();
	ArcnetIO( const std::string& deviceName );
    ~ArcnetIO();

    // public interface
    void enable();
    void disable(void);
    MessagePtr readMessage();
    void writeMessage( const MessagePtr& msg );

    void setNonBlocking ( const bool );

private:
	// Socket initialization
	void createSocket();
	void bindDevToSocket();

	// Data Transimission
	void send ( const uint8_t* msg, unsigned int length ) const;
	void recv ( uint8_t* msg );

	// STORAGE
	bool isEnabled_;

    // Socket information
	int socket_;
	std::string deviceName_;
	struct sockaddr sa_;
};

} // namespace

#endif
