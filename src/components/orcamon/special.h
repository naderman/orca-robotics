#ifndef _ORCAMON_SPECIAL_H_
#define _ORCAMON_SPECIAL_H_

// template <> void 
// attach<orca::GpsPrx,orca::GpsMapGridConsumerPrx,orca::GpsMapGridConsumer,orca::GpsMapGridDataPtr>
void attachGpsMapGrid( const orcaice::Context & context, const std::string & proxyString  );

// template <> void
//  attach<orca::GpsPrx,orca::GpsTimeConsumerPrx,orca::GpsTimeConsumer,orca::GpsTimeDataPtr>
void attachGpsTime( const orcaice::Context & context, const std::string & proxyString  );

#endif
