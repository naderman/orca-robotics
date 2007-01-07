/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>

#include "gpsconsumerI.h"
#include "gpslogger.h"

using namespace std;
using namespace orcalogfactory;

void 
GpsConsumerI::setData(const orca::GpsData& data, const Ice::Current&)
{
    cout << "INFO(gpsconsumerI.cpp): Received GPS data. Telling gpsSlave to log it" << endl << endl;
    logger_->localSetData( data );
}

void 
GpsTimeConsumerI::setData(const orca::GpsTimeData& data, const Ice::Current&)
{
    cout << "INFO(gpsconsumerI.cpp): Received GPS time data. Telling gpsSlave to log it" << endl << endl;
    logger_->localSetData( data );
}

void 
GpsMapGridConsumerI::setData(const orca::GpsMapGridData& data, const Ice::Current&)
{
    cout << "INFO(gpsconsumerI.cpp): Received GPS mapgrid data. Telling gpsSlave to log it" << endl << endl;
    logger_->localSetData( data );
}
