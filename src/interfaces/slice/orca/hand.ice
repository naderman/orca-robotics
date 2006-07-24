/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_HAND_INTERFACE_ICE
#define ORCA2_HAND_INTERFACE_ICE

#include <orca/orca.ice>
#include <orca/bros1.ice>

module orca
{
/*!
    @ingroup interfaces
    @defgroup orca_interface_hand Hand
    @brief Model of a hand, both for sensing and control of a motorised hand

    A representation of computer models of the hand. This is designed for both
    sensed hand data gathered using instrumented gloves (such as the Immersion
    CyberGlove, or the 5th glove) or from a video stream, 
    but also about can represent a robotic hand such as the Barrett Hand or the
    Robotis 9DOF hand, or even just a simple gripper. 

    @{
*/

//! A sequence of angles, usually for joints. 
sequence<float>  AngleSequence;

//! A sequence of torques -- again, torques on the joints. 
sequence<float> TorqueSequence;

//! A sequence of 1D forces

sequence<float> ForceSequence; 


//! Laser configuration data structure
class HandConfig extends OrcaObject
{
    //! Number of fingers in the hand. 

    int numFingers; 

    //! This hand can sense or control abduction.
    //! Abduction measures interfinger angle -- think
    //! of the Vulcan "live long and prosper" thing. 
    bool hasAbduction;
    
    //! This hand has or can sense joint angles
    //! at the first joint, commonly called the 
    //! MCP (metacarpophalangeal) joint. 
   
    bool hasMCPJointAngles;

    //! Can sense joint torques at the MCP joint
    bool hasMCPJointTorque; 

    //! Measures/senses the second joint. 
    //! PIP = Proximal Interphalangeal joint
    bool hasPIPJointAngles; 

    bool hasPIPJointTorque; 

    //! DIP = distal interphalangeal joints
    bool hasDIPJointAngles;
 
    bool hasDIPJointTorques;

    //! Has some indicator of whther the tips of the fingers
    //! are in contact with something. 
    bool hasTipForce; 
   


};


/*!
    Hand data structure
*/
class HandData extends OrcaObject
{
   AngleSequence mcpAngles; // in degrees. 

   AngleSequence pipAngles; // in degrees. 

   AngleSequence dipAngles; // in degrees. 

   AngleSequence abdAngles; // in degrees. 

   TorqueSequence mcpTorque; // in Nm. 

   TorqueSequence pipTorque; // in Nm. 

   TorqueSequence dipTorque; // in Nm. 

   ForceSequence tipForce; // in N. 
    
};

/*!
    @brief Consumer of laser scans
*/
interface HandConsumer
{
    void setData( HandData obj );
};

/*!
    @brief Single-origin scanning range sensor
 */
interface HandProducer
{
    //! Functions for reading from the laser
    nonmutating HandData      getData();
    nonmutating HandConfig    getConfig();


    /*!
     *
     * Mimics IceStorm's subscribe() but without QoS, for now. The
     * implementation may choose to implement the push or use IceStorm. This choice
     * is transparent to the subscriber.
     *
     * @param subscriber The subscriber's proxy.
     *
     * possible additional parameter:
     * preferedPushInterval The subscriber's preference for how often it wants to
     * receive updates [sec]. Provider's ability to fulfil this request may vary.
     *
     * @see unsubscribe
     *
     */
    void subscribe( HandConsumer *subscriber );

    // this is what IceStorm's subscribe function looks like.
    //void subscribe(QoS theQoS, Object* subscriber);

    /**
     *
     * Unsubscribe the given [subscriber].
     *
     * @param subscriber The proxy of an existing subscriber.
     *
     * @see subscribe
     *
    **/
    idempotent void unsubscribe( HandConsumer *subscriber );
};

/*! @} */
}; // module

#endif
