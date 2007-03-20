
#include <stdio.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>   //read and write to ports
#include <string.h>
#include <iostream>

#include <libpcan.h>  //stuff to access the peak can drivers

#include <orcaice/context.h>
#include "../dunc_can_test.h"
#include "../../hwdriver.h"

using namespace orcacan;

int main(void){
    int flags=0;

    orcaice::Context can_context;
    HwDriver::SegwayRmpData test_data;
    std::string output_string;
    HwDriver::SegwayRmpCommand test_command;


    //declare our Can Driver object
    orcacan::PeakCanDriver test(can_context);
 
    //a bogus command???
    test.write(test_command);
    
    
    while(true){ //**??** how should this be escaped?
      test.read(test_data,output_string);
    }
    return 0;


}

