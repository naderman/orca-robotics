/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef  NETWORK_DRIVER_H
#define  NETWORK_DRIVER_H

namespace orcateleop
{

class NetworkDriver
{

public:
    virtual ~NetworkDriver() {};
    
    // Returns 0 on success. Does not throw.
    virtual int enable() = 0;
    
    // Returns 0 on success. Does not throw.
    virtual int disable() = 0;

    virtual void repeatCommand() = 0;

    virtual void processMixedCommand( double longitudinal, bool isLongIncrement, 
        double transverse, bool isTransverseIncrement, 
        double angular, bool isAngularIncrement ) = 0;
    virtual void processIncrementCommand( int longitudinal, int transverse, int angular ) = 0;
    virtual void processRelativeCommand( double longitudinal, double transverse, double angular ) = 0;

protected:
    static void incrementValue( double& value, double delta, double minValue, double maxValue );
    static void setValue( double& value, double newValue, double minValue, double maxValue );
};

} // namespace

#endif
