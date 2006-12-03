/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef ORCANAV_COV_PREDICT_H
#define ORCANAV_COV_PREDICT_H

namespace orcanavutil {

//
// The prediction step of an EKF for the covariance: 
//   P = FPF' + GQG' + sigma
//
// The action is defined in the vehicle's local coordinate system.
//
// [ref: Hugh's notes, esp. slides 146 & 248]
//
void
covPredict( float  poseX,
            float  poseY,
            float  poseT,
            float &Pxx,
            float &Pxy,
            float &Pyy,
            float &Pxt,
            float &Pyt,
            float &Ptt,
            float  actX,
            float  actY,
            float  actT,
            float  velScaleSd,
            float  yawScaleSd,
            float  scaledStabNoisePxx,
            float  scaledStabNoisePyy,
            float  scaledStabNoisePtt );

//
// The prediction step of an EKF for the covariance, assuming known starting point:
//   P = GQG' + sigma
//
// The action is defined in the vehicle's local coordinate system.
//
// [ref: Hugh's notes, esp. slides 146 & 248]
//
void
covPredictFromCertainStart( float  poseX,
                            float  poseY,
                            float  poseT,
                            float &Pxx,
                            float &Pxy,
                            float &Pyy,
                            float &Pxt,
                            float &Pyt,
                            float &Ptt,
                            float  actX,
                            float  actY,
                            float  actT,
                            float  velScaleSd,
                            float  yawScaleSd,
                            float  scaledStabNoisePxx,
                            float  scaledStabNoisePyy,
                            float  scaledStabNoisePtt );


// for debugging
void
printP( float Pxx,
        float Pxy,
        float Pyy,
        float Pxt,
        float Pyt,
        float Ptt );

}

#endif
