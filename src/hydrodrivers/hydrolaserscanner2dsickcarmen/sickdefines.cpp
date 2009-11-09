#include "sickdefines.h"
#include <cstring>
#include <iostream>
#include <sstream>
#include <cstdio>

using namespace std;

laser_settings_t::laser_settings_t( const char *device, 
                                    const char *laserType, // eg "LMS"
                                    int         baudrate,
                                    double      resolutionInDegrees,
                                    range_res_t rangeResolution )
    : device_name(device),
      type(LMS),
      range_res(rangeResolution),
      range_dist(RANGE80M),
      laser_num(0),
      detect_baudrate(true),
      use_highspeed(false),
      start_baudrate(9600),
      set_baudrate(baudrate),
      databits(8),                         
      stopbits(1),                         
      parity(N),                           
      swf(0),                              
      hwf(0),                              
      angle_range(180),                    
      angle_resolution(RES_1_00_DEGREE),   
      laser_flipped(0)                    
{
    if(strcmp(laserType, "LMS") == 0)
    {
        type = LMS;
        password = LMS_PASSWORD;
        parity = N;
    }
    else if(strcmp(laserType, "PLS") == 0)
    {
        type = PLS;
        password = PLS_PASSWORD;
        parity = E;
    }
    if (resolutionInDegrees == 0.25) 
    {
        angle_resolution = RES_0_25_DEGREE;
        angle_range = 100;
    }
    else if (resolutionInDegrees == 0.5)
        angle_resolution = RES_0_50_DEGREE;
    else
        angle_resolution = RES_1_00_DEGREE;

    if ( detect_baudrate )
        start_baudrate = 9600;

    if ( set_baudrate == 500000 )
        use_highspeed = true;

    if(angle_range == 100) {
        if(angle_resolution == RES_1_00_DEGREE)
            num_values = 101;
        else if(angle_resolution == RES_0_50_DEGREE)
            num_values = 201;
        else if(type == LMS)
            num_values = 401;
        else
            throw( std::string( "ang-res=0.25 is not valid for this laser!" ) );
    }
    else {
        if(angle_resolution == RES_1_00_DEGREE)
            num_values = 181;
        else if(angle_resolution == RES_0_50_DEGREE)
            num_values = 361;
        else {
            throw std::string( "ang-res=0.25 and ang-range=180 is not valid!" );
        }
    }
    if(type == PLS) {
        if(angle_range == 100) {
            throw std::string( "type = PLS and ang-range=100 is not valid!" );
        } 
    }

    checkValid();
    // print();
}

void
laser_settings_t::checkValid() const
{
    /*********************** TYPE CHECKING **************************/
    if ( type != PLS && type != LMS )
    {
        throw ( std::string("Invalid type.") );
    }
    
    /*********************** START BAUDRATE **************************/
    if(start_baudrate != 9600 &&
       start_baudrate != 19200 &&
       start_baudrate != 38400 &&
       start_baudrate != 500000) 
    {
        stringstream ss;
        ss << "start baudrate = "<<start_baudrate<<" is not valid!";
        throw ss.str();
    }

    /*********************** SET BAUDRATE **************************/
    if(set_baudrate != 9600 &&
       set_baudrate != 19200 &&
       set_baudrate != 38400 &&
       set_baudrate != 500000) 
    {
        stringstream ss;
        ss << "set baudrate = "<<set_baudrate<<" is not valid!";
        throw ss.str();
    } 
  
    /*********************** NUM VALUES **************************/
    if(angle_range != 180 && 
       angle_range != 100) 
    {
        stringstream ss;
        ss << "angle range = "<<angle_range<<" is not valid!";
        throw ss.str();
    }
  
    /************************** ANGLE RANGE ************************/
    if(angle_range == 100) {
        if ( num_values != 101 &&
             num_values != 201 &&
             num_values != 401 )
            throw( std::string( "ang-res=0.25 is not valid for this laser!" ) );
    }
    else {
        if ( num_values != 181 &&
             num_values != 361 )
            throw std::string( "ang-res=0.25 and ang-range=180 is not valid!" );
    }
    if(type == PLS) {
        if(angle_range == 100) {
            throw std::string( "type = PLS and ang-range=100 is not valid!" );
        } 
    }
}

void
laser_settings_t::print() const
{
    printf("Parameters:\n");
    printf("\tdevice_name:      %s\n",device_name.c_str());
    printf("\ttype:             %s\n",(type==PLS)?"PLS":((type==LMS)?"LMS":"unknown"));
    printf("\trange_res:        %s\n",((range_res)==CM?"CM":((range_res==MM)?"MM":((range_res==DM)?"DM":"unknown"))));
    printf("\trange_dist:       %s\n",((range_dist==RANGE80M)?"RANGE80M":((range_dist==RANGE160M)?"RANGE160M":((range_dist==RANGE320M)?"RANGE320M":"unknown"))));
    printf("\tpassword:         %s\n",password.c_str());
    printf("\tlaser_num:        %d\n",laser_num);
    printf("\tdetect_baudrate:  %d\n",detect_baudrate);
    printf("\tuse_highspeed:    %d\n",use_highspeed);
    printf("\tstart_baudrate:   %d\n",start_baudrate);
    printf("\tset_baudrate:     %d\n",set_baudrate);
    printf("\tdatabits:         %d\n",databits);
    printf("\tstopbits:         %d\n",stopbits);
    printf("\tparity:           %s\n",((parity)==N?"N":((parity)==E?"E":((parity==O)?"O":"unknown"))));
    printf("\tswf:              %d\n",swf);
    printf("\thwf:              %d\n",hwf);
    printf("\tangle_range:      %d\n",angle_range);
    printf("\tangle_resolution: %s\n",((angle_resolution==RES_1_00_DEGREE)?"RES_1_00_DEGREE":((angle_resolution==RES_0_50_DEGREE)?"RES_0_50_DEGREE":((angle_resolution==RES_0_25_DEGREE)?"RES_0_25_DEGREE":"unknown"))));
    printf("\tnum_values:       %d\n",num_values);
    printf("\tlaser_flipped:    %d\n",laser_flipped);
}
