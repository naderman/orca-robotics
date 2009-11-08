/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDRO_INTERFACES_HUMANINPUT2D_H
#define HYDRO_INTERFACES_HUMANINPUT2D_H

#include <hydroutil/context.h>

namespace hydrointerfaces 
{

/*!
    @ingroup hydro_drivers
    @defgroup hydro_drivers_humaninput2d HumanInput2d drivers
    @brief Implementations of HumanInput2d hardware interface
*/
/*!
    @ingroup hydro_interfaces
    @defgroup hydro_interface_humaninput2d HumanInput2d
    @brief Human input for position or velocity control of 2D platforms.

    @{
*/

/*!

@brief Abstract interface class for human teleoperation input.

@author Alex Makarenko

*/
class SOEXPORT HumanInput2d
{

public:

    static const int TIMEOUT=-2;

    //! Command structure. All values are interpreted as either absolute command
    //! or incremental, depending on value of the @c isIncrement* switch.
    //!
    //! If the increment switch is TRUE, the value is interpreted
    //! as increment in command. The increment is typically an integer but this is not
    //! inforced.
    //! 
    //! Otherwise, the value is interpreted as the absolute command command
    //! in the range from -1.0 to +1.0, corresponding to maximum allowed values in both
    //! directions.
    //! 
    //! To indicate "no change" set the increment switch to TRUE and the command 
    //! to 0.0
    class SOEXPORT Command
    {   
    public: 
        //! Longitudinal command.
        double longitudinal;
        //! Switch between incremental and absolute command for longitudinal command.
        bool isLongIncrement;
    
        //! Transverse command.
        double transverse;
        //! Switch between incremental and absolute command for transverse command.
        bool isTransverseIncrement;

        //! Angular command.
        double angular;
        //! Switch between incremental and absolute command for angular command.
        bool isAngularIncrement;

        void resetIncremental();
        void resetAbsolute();
        std::string toString() const;
    };

    virtual ~HumanInput2d() {};
    
    //! Returns 0 on success. Does not throw.
    virtual int enable()=0;
    
    //! Returns 0 on success. Does not throw.
    virtual int disable()=0;

    //! Non-Blocking read. 
    //! Returns 0 on success, TIMEOUT if no data is received, anything else is an error. Does not throw.
    virtual int read( Command& command )=0;
};

//! Helper class to instantiate drivers
class SOEXPORT HumanInput2dFactory {
public:
    virtual ~HumanInput2dFactory() {};
    //! Instantiates a driver and returns a pointer to it.
    virtual HumanInput2d *createDriver( const hydroutil::Context &context ) const=0;
};

/*! @} */
} // namespace

// Function for dynamically instantiating drivers.
// A driver must have a function like so:
// extern "C" {
//     hydrointerfaces::HumanInput2dFactory *createHumanInput2dDriverFactory();
// }
typedef hydrointerfaces::HumanInput2dFactory *HumanInput2dDriverFactoryMakerFunc();

#endif
