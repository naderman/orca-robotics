/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef _ORCAMON_SPECIAL_H_
#define _ORCAMON_SPECIAL_H_

// template <> void 
// attach<orca::GpsPrx,orca::GpsMapGridConsumerPrx,orca::GpsMapGridConsumer,orca::GpsMapGridDataPtr>
void attachGpsMapGrid( const orcaice::Context & context, const std::string & proxyString  );

// template <> void
//  attach<orca::GpsPrx,orca::GpsTimeConsumerPrx,orca::GpsTimeConsumer,orca::GpsTimeDataPtr>
void attachGpsTime( const orcaice::Context & context, const std::string & proxyString  );

#endif
