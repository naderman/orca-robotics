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

#include "arcnetio.h"

// Stuff for arcnet communications
#include <unistd.h>
#include <arpa/inet.h>
#include <net/if_arp.h>
#include <netpacket/packet.h>
#include <linux/if_arcnet.h>
#include <pcap.h>
#include <sys/time.h>

#include <fcntl.h>
#include <assert.h>
#include <signal.h>

// For error information
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <sstream>
#include <IceUtil/IceUtil.h>
#include <hydrointerfaces/mhipa10.h> // for Exception

#define THROW_ERROR(msg)	std::stringstream ss;																\
							ss << __FILE__ << ": " << __FUNCTION__ << "() [" << deviceName_ << "]: " <<  msg;	\
							throw hydrointerfaces::MitsubishiPA10::Exception( ss.str() );
							
#define TRY_STD_FUNC(expr)	if ( (expr) < 0 ) {					\
								THROW_ERROR(strerror(errno));	\
							}

namespace mhipa10arc {

void sig_alrm( int signo )
{
	// Do nothing.. just handle the signal
	return;
}

ArcnetIO::ArcnetIO() :
	ALARM_TIMEOUT(10000), // Alarm timeout = 10 ms = Duration of PA10's internal watchdog timer
	SELECT_TIMEOUT(8000), // Select timeout = 8 ms = 4 PA10 comm cycles
	isEnabled_(false),
	socket_ ( -1 ),
	deviceName_("NO_NAME")
{
	memset ( &sa_, 0, sizeof ( sa_ ) );

	// We weren't passed a device name so try using pcap to find the arcnet device
	pcap_if_t* devpointer = NULL;
	char errbuf[PCAP_ERRBUF_SIZE];
	if ( pcap_findalldevs(&devpointer, errbuf) == 0 )
	{
		// We will use the first valid arcnet device we come across
		while ( devpointer != NULL )
		{
			char devClass[4] = {'\0'};
			memcpy(devClass, devpointer->name, 3);
			if ( strcmp("arc",devClass) == 0 )
			{
				deviceName_ = devpointer->name;
				break;
			}
			devpointer = devpointer->next;
		}
	} else {
		THROW_ERROR(errbuf);
	}
}

ArcnetIO::ArcnetIO( const std::string& deviceName ) :
	ALARM_TIMEOUT(10000), // Alarm timeout = 10 ms = Duration of PA10's internal watchdog timer
	SELECT_TIMEOUT(8000), // Select timeout = 8 ms = 4 PA10 comm cycles
	isEnabled_(false),
	socket_ ( -1 ),
	deviceName_( deviceName )
{
	memset ( &sa_, 0, sizeof ( sa_ ) );
}

ArcnetIO::~ArcnetIO()
{
    disable();
}

void ArcnetIO::enable( )
{
	if ( isEnabled_ ) return;

	// Create and bind to socket
	createSocket();
	bindDevToSocket();

	// Make socket non-blocking
	setNonBlocking(true);

	// Register our alarm signal handler so we can timeout bad reads
	signal(SIGALRM,sig_alrm);

	isEnabled_ = true;
}

void ArcnetIO::disable()
{
	if ( !isEnabled_ ) return;

	// Reset socket stuff
	memset ( &sa_, 0, sizeof ( sa_ ) );
	TRY_STD_FUNC( close(socket_) );
	socket_ = -1;

	isEnabled_ = false;
}


MessagePtr ArcnetIO::readMessage()
{
	if ( !isEnabled_ )
	{
		THROW_ERROR("Arcnet subsystem has not been enabled.");
	}

	MessagePtr msg( new Message( Message::max_msg_length ) );
	this->recv( msg->msg() );
	msg->timeStamp(); // Record when message was received
	return msg;
}

void ArcnetIO::writeMessage( const MessagePtr& msg )
{
	if ( !isEnabled_ )
	{
		THROW_ERROR("Arcnet subsystem has not been enabled.");
	}

	this->send( msg->msg(), msg->length() );
}

void ArcnetIO::createSocket()
{
	// Open the socket
	//	Notes: 	* htons(ETH_P_ALL) - all protocols are received
	//			* PF_PACKET - packet interface on device level
	//			* SOCK_RAW packets are passed to and from the device
	//			 	driver without any changes in the packet data.
	//			* SOCK_PACKET is obsolete (replaced by SOCK_RAW)
	// sock = socket(PF_PACKET, SOCK_PACKET, htons(ETH_P_ALL)); // OBSOLETE
	TRY_STD_FUNC( socket_ = socket(PF_PACKET, SOCK_PACKET, htons(ETH_P_ALL)) );
}

void ArcnetIO::bindDevToSocket()
{
	if ( deviceName_ == "NO_NAME" )
	{
		THROW_ERROR("A suitable arcnet device could not be found by pcap library.");
	}

	// Bind to the socket
	sa_.sa_family = ARPHRD_ARCNET;
	strcpy( sa_.sa_data, deviceName_.c_str() );

	TRY_STD_FUNC( bind(socket_, &sa_, sizeof(sa_) ) );
}

void ArcnetIO::send ( const uint8_t* msg, unsigned int length ) const
{
	// Do some setup for select()
	timeval timeout; timeout.tv_sec = 0; timeout.tv_usec = SELECT_TIMEOUT;
	fd_set socks; // Socket file descriptors we want to wake up for, using select()
	FD_ZERO(&socks); FD_SET(socket_,&socks);
	int writesocks = -1;
	TRY_STD_FUNC( (writesocks = select(socket_+1, NULL, &socks, NULL, &timeout)) );
		
	if ( writesocks == 0 )
	{
		// Not normally an error but indicates nothing ready to read. Throw an error?
		THROW_ERROR( "Nothing ready to write. But how?" );
	}
	
	// Should have a valid socket for writing to
	TRY_STD_FUNC( sendto(socket_, msg, length, 0, &sa_, sizeof(sa_)) );
}

void ArcnetIO::recv ( uint8_t* msg )
{
	socklen_t saLength = sizeof(sa_);

	// Do some setup for select()
	timeval timeout; timeout.tv_sec = 0; timeout.tv_usec = SELECT_TIMEOUT;
	fd_set socks; // Socket file descriptors we want to wake up for, using select()
	FD_ZERO(&socks); FD_SET(socket_,&socks);
	int readsocks = -1;
	TRY_STD_FUNC( (readsocks = select(socket_+1, &socks, NULL, NULL, &timeout)) );
		
	if ( readsocks == 0 )
	{
		// Not normally an error but indicates nothing ready to read. Throw an error?
		THROW_ERROR( "Nothing ready to read. But how?" );
	}
	
	// If we get this far, we have a valid socket that is ready for reading.
	ualarm( ALARM_TIMEOUT, 0 ); // Start alarm timer
	if ( recvfrom(socket_, msg, Message::max_msg_length, 0, &sa_, &saLength) < 0 )
	{
		if (errno == EINTR)
		{
			THROW_ERROR("PA-10 seems unresponsive. Is it turned on?");
		} else {
			ualarm(0, 0);
			THROW_ERROR(strerror(errno));
		}
	} else {
		// reset alarm
		ualarm(0, 0);
	}
}

void ArcnetIO::setNonBlocking ( const bool b )
{
	int opts;

	TRY_STD_FUNC( opts = fcntl(socket_, F_GETFL) );

	if ( b )
		opts = ( opts | O_NONBLOCK );
	else
		opts = ( opts & ~O_NONBLOCK );

	TRY_STD_FUNC( fcntl( socket_, F_SETFL, opts ) );
}

}
