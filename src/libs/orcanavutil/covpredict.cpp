/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include "covpredict.h"
#include <cmath>
#include <iostream>

using namespace std;


namespace orcanavutil {

//
// This uses the initial values of P, and modifies them according to F
//
// NOTE: These calcs come from ~a.brooks/matlab/vehicle_prediction/symbolicCalc.m
//
void
PgetsFPF( double &Pxx,
          double &Pxy,
          double &Pyy,
          double &Pxt,
          double &Pyt,
          double &Ptt,
          double  actX,
          double  actY,
          double  actT,
          double  sTheta,
          double  cTheta )
{
    // A = -actX*sPsi-actY*cPsi
    // B =  actX*cPsi-actY*sPsi
    // 
    // C = Pxt+A*Ptt
    // D = Pyt+B*Ptt
    // 
    // FPF =
    // [ Pxx+A*Pxt+C*A, Pxy+A*Pyt+C*B, C   ]
    // [ Pxy+B*Pxt+D*A, Pyy+B*Pyt+D*B, D   ]
    // [       C,         D,           Ptt ]


    const double A = -actX*sTheta-actY*cTheta;
    const double B =  actX*cTheta-actY*sTheta;

    const double C = Pxt+A*Ptt;
    const double D = Pyt+B*Ptt;

    const double FPFxx = Pxx+A*Pxt+C*A;
    const double FPFxy = Pxy+B*Pxt+D*A;
    const double FPFyy = Pyy+B*Pyt+D*B;
    const double FPFxt = C;
    const double FPFyt = D;
    const double FPFtt = Ptt;

    Pxx = FPFxx;
    Pxy = FPFxy;
    Pyy = FPFyy;
    Pxt = FPFxt;
    Pyt = FPFyt;
    Ptt = FPFtt;
}

//
// NOTE: These calcs come from ~a.brooks/matlab/vehicle_prediction/symbolicCalc.m
//
void
addGQG( double &Pxx,
        double &Pxy,
        double &Pyy,
        double &Pxt,
        double &Pyt,
        double &Ptt,
        double  actX,
        double  actY,
        double  actT,
        double  velScaleSd,
        double  yawScaleSd,
        double  sTheta,
        double  cTheta )
{
    // E = velScaleSd^2*actX^2
    // F = velScaleSd^2*actY^2
    //
    // GQG =
    // [ cPsi*E*cPsi+sPsi*F*sPsi, cPsi*E*sPsi-sPsi*F*cPsi,           0          ]
    // [ sPsi*E*cPsi-cPsi*F*sPsi, sPsi*E*sPsi+cPsi*F*cPsi,           0          ]
    // [           0,                       0,                yawScale^2*actT^2 ]

    double E = velScaleSd*actX;
    E = E*E;
    double F = velScaleSd*actY;
    F = F*F;

    double GQGxx = cTheta*E*cTheta+sTheta*F*sTheta;
    double GQGxy = cTheta*E*sTheta-sTheta*F*cTheta;
    double GQGxt = 0;
    double GQGyy = sTheta*E*sTheta+cTheta*F*cTheta;
    double GQGyt = 0;
    double GQGtt = yawScaleSd*actT;
    GQGtt = GQGtt*GQGtt;

    Pxx += GQGxx;
    Pxy += GQGxy;
    Pxt += GQGxt;
    Pyy += GQGyy;
    Pyt += GQGyt;
    Ptt += GQGtt;
}

//
// NOTE: These calcs come from ~a.brooks/matlab/vehicle_prediction/symbolicCalc.m
//
void
addStabilisingNoise( double &Pxx,
                     double &Pxy,
                     double &Pyy,
                     double &Pxt,
                     double &Pyt,
                     double &Ptt,
                     double  scaledStabNoisePxx,
                     double  scaledStabNoisePyy,
                     double  scaledStabNoisePtt )
{
    Pxx += scaledStabNoisePxx;
    Pyy += scaledStabNoisePyy;
    Ptt += scaledStabNoisePtt;
}


void
covPredict( double  poseX,
            double  poseY,
            double  poseT,
            double &Pxx,
            double &Pxy,
            double &Pyy,
            double &Pxt,
            double &Pyt,
            double &Ptt,
            double  actX,
            double  actY,
            double  actT,
            double  velScaleSd,
            double  yawScaleSd,
            double  scaledStabNoisePxx,
            double  scaledStabNoisePyy,
            double  scaledStabNoisePtt )
{
    const double cTheta = cos(poseT);
    const double sTheta = sin(poseT);

    // P = FPF' + GQG' + sigma;

    PgetsFPF( Pxx, Pxy, Pyy, Pxt, Pyt, Ptt, actX, actY, actT, sTheta, cTheta );

//     cout<<"TRACE(covpredict.cpp): FPF: " << endl;
//     printP( Pxx, Pxy, Pyy, Pxt, Pyt, Ptt );

    addGQG(   Pxx, Pxy, Pyy, Pxt, Pyt, Ptt, actX, actY, actT, velScaleSd, yawScaleSd, sTheta, cTheta );
    addStabilisingNoise( Pxx, Pxy, Pyy, Pxt, Pyt, Ptt, scaledStabNoisePxx, scaledStabNoisePyy, scaledStabNoisePtt );
}

void
covPredictFromCertainStart( double  poseX,
                            double  poseY,
                            double  poseT,
                            double &Pxx,
                            double &Pxy,
                            double &Pyy,
                            double &Pxt,
                            double &Pyt,
                            double &Ptt,
                            double  actX,
                            double  actY,
                            double  actT,
                            double  velScaleSd,
                            double  yawScaleSd,
                            double  scaledStabNoisePxx,
                            double  scaledStabNoisePyy,
                            double  scaledStabNoisePtt )
{
    const double cTheta = cos(poseT);
    const double sTheta = sin(poseT);

    // P = 0 + GQG' + sigma;

    Pxx = 0.0;
    Pxy = 0.0;
    Pyy = 0.0;
    Pxt = 0.0;
    Pyt = 0.0;
    Ptt = 0.0;

    addGQG(   Pxx, Pxy, Pyy, Pxt, Pyt, Ptt, actX, actY, actT, velScaleSd, yawScaleSd, sTheta, cTheta );
    addStabilisingNoise( Pxx, Pxy, Pyy, Pxt, Pyt, Ptt, scaledStabNoisePxx, scaledStabNoisePyy, scaledStabNoisePtt );
}


void
printP( double Pxx,
        double Pxy,
        double Pyy,
        double Pxt,
        double Pyt,
        double Ptt )
{
    cout<<"TRACE(testcovpredict.cpp): P:  " << Pxx << endl;
    cout<<"TRACE(testcovpredict.cpp): P:  " << Pxy << "\t " << Pyy << endl;
    cout<<"TRACE(testcovpredict.cpp): P:  " << Pxt << "\t " << Pyt << "\t " << Ptt << endl;
}


}
