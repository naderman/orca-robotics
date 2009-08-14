/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#include <hydronavutil/covpredict.h>
#include <iostream>
#include <cstdlib>
#include <gbxutilacfr/mathdefs.h>

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
        cout << "ERROR(testcovpredict.cpp): xx mismatch: mPxx=" << mPxx << endl;
    if ( !NEAR( Pxy, mPxy, e ) )
        cout << "ERROR(testcovpredict.cpp): xy mismatch: mPxy=" << mPyy << endl;
    if ( !NEAR( Pyy, mPyy, e ) )
        cout << "ERROR(testcovpredict.cpp): yy mismatch: mPyy=" << mPyy << endl;
    if ( !NEAR( Pxt, mPxt, e ) )
        cout << "ERROR(testcovpredict.cpp): xt mismatch: mPxt=" << mPxt << endl;
    if ( !NEAR( Pyt, mPyt, e ) )
        cout << "ERROR(testcovpredict.cpp): yt mismatch: mPyt=" << mPyt << endl;
    if ( !NEAR( Ptt, mPtt, e ) )
        cout << "ERROR(testcovpredict.cpp): tt mismatch: mPtt=" << mPtt << endl;

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

    double velScaleCov = 0.3*0.3;
    double yawScaleCov = 0.3*0.3;

    double stabNoisexxCov = 0.01;
    double stabNoiseyyCov = 0.01;
    double stabNoisettCov = 0.01;

    double dt = 1.0;

    double scaledStabNoisexxCov = stabNoisexxCov * dt;
    double scaledStabNoiseyyCov = stabNoiseyyCov * dt;
    double scaledStabNoisettCov = stabNoisettCov * dt;

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

    hydronavutil::covPredict( poseX,
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
                              velScaleCov,
                              yawScaleCov,
                              scaledStabNoisexxCov,
                              scaledStabNoiseyyCov,
                              scaledStabNoisettCov );
                             
    cout<<"TRACE(testcovpredict.cpp): updated P:" << endl;
    hydronavutil::printP( Pxx, Pxy, Pyy, Pxt, Pyt, Ptt );

    compareAgainstMatlab( Pxx,
                          Pxy,
                          Pyy,
                          Pxt,
                          Pyt,
                          Ptt,
                          1.1,
                          0,
                          1.02,
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

    hydronavutil::covPredict( poseX,
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
                              velScaleCov,
                              yawScaleCov,
                              scaledStabNoisexxCov,
                              scaledStabNoiseyyCov,
                              scaledStabNoisettCov );
                             
    cout<<"TRACE(testcovpredict.cpp): updated P:" << endl;
    hydronavutil::printP( Pxx, Pxy, Pyy, Pxt, Pyt, Ptt );

    compareAgainstMatlab( Pxx,
                          Pxy,
                          Pyy,
                          Pxt,
                          Pyt,
                          Ptt,
                          1.119092974268257,
                          0.004161468365471,
                          1.100907025731743,
                          -0.013817732906760,
                          -0.003011686789398,
                          0.114247779607694 );

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

    hydronavutil::covPredict( poseX,
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
                             velScaleCov,
                             yawScaleCov,
                             scaledStabNoisexxCov,
                             scaledStabNoiseyyCov,
                             scaledStabNoisettCov );

    cout<<"TRACE(testcovpredict.cpp): updated P:" << endl;
    hydronavutil::printP( Pxx, Pxy, Pyy, Pxt, Pyt, Ptt );

    compareAgainstMatlab( Pxx,
                          Pxy,
                          Pyy,
                          Pxt,
                          Pyt,
                          Ptt,
                          0.014077503666032,
                          -0.004944857073662,
                          0.016318505071634,
                          -0.000077503069551,
                          0.000116837311110,
                          0.010474717300000 );

    cout<<"TRACE(testcovpredict.cpp): Test PASSED!" << endl;

    return 0;
}
