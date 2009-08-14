/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef ORCANAV_COV_PREDICT_H
#define ORCANAV_COV_PREDICT_H

namespace hydronavutil {

//
// The prediction step of an EKF for the covariance: 
//   P = FPF' + GQG' + sigma
//
// The action is defined in the vehicle's local coordinate system.
//
// [ref: Hugh's notes, esp. slides 146 & 248]
//
void
covPredict( double  poseX,
            double  poseY,
            double  poseT,
            double &Pxx,
            double &Pxy,
            double &Pxt,
            double &Pyy,
            double &Pyt,
            double &Ptt,
            double  actX,
            double  actY,
            double  actT,
            double  velScaleCov,
            double  yawScaleCov,
            double  scaledStabNoisePxx,
            double  scaledStabNoisePyy,
            double  scaledStabNoisePtt );

//
// The prediction step of an EKF for the covariance, assuming known starting point:
//   P = GQG' + sigma
//
// The action is defined in the vehicle's local coordinate system.
//
// [ref: Hugh's notes, esp. slides 146 & 248]
//
void
covPredictFromCertainStart( double  poseX,
                            double  poseY,
                            double  poseT,
                            double &Pxx,
                            double &Pxy,
                            double &Pxt,
                            double &Pyy,
                            double &Pyt,
                            double &Ptt,
                            double  actX,
                            double  actY,
                            double  actT,
                            double  velScaleCov,
                            double  yawScaleCov,
                            double  scaledStabNoisePxx,
                            double  scaledStabNoisePyy,
                            double  scaledStabNoisePtt );


// for debugging
void
printP( double Pxx,
        double Pxy,
        double Pyy,
        double Pxt,
        double Pyt,
        double Ptt );

}

#endif
