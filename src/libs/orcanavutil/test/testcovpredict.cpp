/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include <orcanavutil/covpredict.h>
#include <iostream>
#include <hydroutil/mathdefs.h>

using namespace std;

void
compareAgainstMatlab( double Pxx,
                      double Pxy,
                      double Pyy,
                      double Pxt,
                      double Pyt,
                      double Ptt,
                      double mPxx,
                      double mPxy,
                      double mPyy,
                      double mPxt,
                      double mPyt,
                      double mPtt )
{
    double e = 1e-6;


    if ( !NEAR( Pxx, mPxx, e ) )
        cout << "ERROR(testcovpredict.cpp): xx mismatch." << endl;
    if ( !NEAR( Pxy, mPxy, e ) )
        cout << "ERROR(testcovpredict.cpp): xy mismatch." << endl;
    if ( !NEAR( Pyy, mPyy, e ) )
        cout << "ERROR(testcovpredict.cpp): yy mismatch." << endl;
    if ( !NEAR( Pxt, mPxt, e ) )
        cout << "ERROR(testcovpredict.cpp): xt mismatch." << endl;
    if ( !NEAR( Pyt, mPyt, e ) )
        cout << "ERROR(testcovpredict.cpp): yt mismatch." << endl;
    if ( !NEAR( Ptt, mPtt, e ) )
        cout << "ERROR(testcovpredict.cpp): tt mismatch." << endl;

    if ( !NEAR( Pxx, mPxx, e ) ||
         !NEAR( Pxy, mPxy, e ) ||
         !NEAR( Pyy, mPyy, e ) ||
         !NEAR( Pxt, mPxt, e ) ||
         !NEAR( Pyt, mPyt, e ) ||
         !NEAR( Ptt, mPtt, e )  )
    {
        cout << "ERROR(testcovpredict.cpp): didn't match MATLAB result." << endl;
        exit(1);
    }
}

int main()
{
    double Pxx;
    double Pxy;
    double Pyy;
    double Pxt;
    double Pyt;
    double Ptt;

    double velScale = 0.3*0.3;
    double yawScale = 0.3*0.3;

    double stabNoisexx = 0.01;
    double stabNoiseyy = 0.01;
    double stabNoisett = 0.01;

    double dt = 1.0;

    double scaledStabNoisexx = stabNoisexx * dt;
    double scaledStabNoiseyy = stabNoiseyy * dt;
    double scaledStabNoisett = stabNoisett * dt;

    double poseX;
    double poseY;
    double poseT;

    double actX;
    double actY;
    double actT;

    // ------------------------------------------------------------
    cout<<"------------------------1--------------------------" << endl;

    Pxx = 1.0;
    Pxy = 0.0;
    Pyy = 1.0;
    Pxt = 0.0;
    Pyt = 0.0;
    Ptt = 0.01;

    poseX = 0.0;
    poseY = 0.0;
    poseT = 0.0;

    actX = 1.0;
    actY = 0.0;
    actT = 0.0;

    orcanavutil::covPredict( poseX,
                             poseY,
                             poseT,
                             Pxx,
                             Pxy,
                             Pxt,
                             Pyy,
                             Pyt,
                             Ptt,
                             actX,
                             actY,
                             actT,
                             velScale,
                             yawScale,
                             scaledStabNoisexx,
                             scaledStabNoiseyy,
                             scaledStabNoisett );
                             
    cout<<"TRACE(testcovpredict.cpp): updated P:" << endl;
    orcanavutil::printP( Pxx, Pxy, Pyy, Pxt, Pyt, Ptt );

    compareAgainstMatlab( Pxx,
                          Pxy,
                          Pyy,
                          Pxt,
                          Pyt,
                          Ptt,
                          1.01810000000000,
                          0,
                          1.02000000000000,
                          0,
                          0.01000000000000,
                          0.02000000000000 );

    // ------------------------------------------------------------
    cout<<"-------------------------2-------------------------" << endl;

    Pxx = 1.0;
    Pxy = 0.0;
    Pyy = 1.0;
    Pxt = 0.0;
    Pyt = 0.0;
    Ptt = 0.01;

    poseX = 1.0;
    poseY = 1.0;
    poseT = 1.0;

    actX = 1.0;
    actY = 1.0;
    actT = M_PI/3.0;

    orcanavutil::covPredict( poseX,
                             poseY,
                             poseT,
                             Pxx,
                             Pxy,
                             Pxt,
                             Pyy,
                             Pyt,
                             Ptt,
                             actX,
                             actY,
                             actT,
                             velScale,
                             yawScale,
                             scaledStabNoisexx,
                             scaledStabNoiseyy,
                             scaledStabNoisett );
                             
    cout<<"TRACE(testcovpredict.cpp): updated P:" << endl;
    orcanavutil::printP( Pxx, Pxy, Pyy, Pxt, Pyt, Ptt );

    compareAgainstMatlab( Pxx,
                          Pxy,
                          Pyy,
                          Pxt,
                          Pyt,
                          Ptt,
                          1.03719297426826,
                          0.00416146836547,
                          1.01900702573174,
                          -0.01381773290676,
                          -0.00301168678940,
                          0.02888264396098 );

    // ------------------------------------------------------------
    cout<<"--------------------------3------------------------" << endl;

    Pxx =  0.00222390000000;
    Pxy = -0.00254080000000;
    Pyy =  0.00318266000000;
    Pxt = -0.00009060230000;
    Pyt =  0.00010671400000;
    Ptt =  0.00029902200000;

    poseX =  2.06469000000000;
    poseY = -0.26674700000000;
    poseT = -0.91415100000000;

    actX = 0.05536410000000;
    actY = 0.00007195160000;
    actT = 0.00195217000000;

    orcanavutil::covPredict( poseX,
                             poseY,
                             poseT,
                             Pxx,
                             Pxy,
                             Pxt,
                             Pyy,
                             Pyt,
                             Ptt,
                             actX,
                             actY,
                             actT,
                             velScale,
                             yawScale,
                             scaledStabNoisexx,
                             scaledStabNoiseyy,
                             scaledStabNoisett );

    cout<<"TRACE(testcovpredict.cpp): updated P:" << endl;
    orcanavutil::printP( Pxx, Pxy, Pyy, Pxt, Pyt, Ptt );

    compareAgainstMatlab( Pxx,
                          Pxy,
                          Pyy,
                          Pxt,
                          Pyt,
                          Ptt,
                          0.01222578838007,
                          -0.00255075369686,
                          0.01320580374244,
                          -0.00007750306955,
                          0.00011683731111,
                          0.01029905286884 );

    cout<<"TRACE(testcovpredict.cpp): Test PASSED!" << endl;

    return 0;
}
