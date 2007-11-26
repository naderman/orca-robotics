/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp, 
 *                         Duncan Mercer
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_ESTOP_CONSUMER_I_H
#define ORCA_ESTOP_CONSUMER_I_H

#include <iostream>
#include <orca/estop.h>
#include <orcaice/orcaice.h>
#include "hwhandler.h"

namespace segwayrmp{


class EStopConsumerI : public orca::EStopConsumer
{
public:  
    EStopConsumerI ( HwHandler& hwHandler ): hwHandler_(hwHandler){};
    ~EStopConsumerI() {};

    // Pass the data straight to the HwHandler to deal with
    virtual void setData(const orca::EStopData& data, const Ice::Current&){
        if(data.hasFaultCondition)
            {hwHandler_.setEStopFaultStatus(ESS_FAULT);}
        else{hwHandler_.setEStopFaultStatus(ESS_NO_FAULT);}
        std::cout << "EStopCallback called"<<std::endl;
    }

private:
    HwHandler& hwHandler_;
    
};

}
#endif
