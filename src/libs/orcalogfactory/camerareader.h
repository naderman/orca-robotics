/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Alen Alempijevic
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_ORCALOGFACTORY_CAMERA_READER_H
#define ORCA_ORCALOGFACTORY_CAMERA_READER_H

#include <orcalog/logreader.h>
#include <orca/camera.h>

#ifdef OPENCV_FOUND
    #include <cv.h>
#endif

namespace orcalogfactory
{

class CameraReader
{
public:
    CameraReader();
    virtual ~CameraReader();

    void readFromFile(  std::ifstream *file, 
                        const std::string &format,
                        orcaice::Context context,
                        orca::CameraDescriptionPtr &obj );

    void readFromFile(  std::ifstream     *file, 
                        const std::string &format,
                        orcaice::Context   context,
                        orca::CameraDataPtr   &obj );

    void loadJpegData( const std::string &format,
                       orca::CameraData   &obj );

    
    // from Orca object

/*
    virtual ::orca::CameraData getData(const ::Ice::Current& ) const;

    virtual void subscribe(const ::orca::CameraConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    virtual void unsubscribe(const ::orca::CameraConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    virtual void replayData( int index, bool isTest=false );
*/

private:

    #ifdef OPENCV_FOUND
        IplImage* cvImage_;
    #endif

/*
    
    orca::CameraData data_;
    
    // binary file protocol specifics
    void loadDataIce( int index );
    void loadDataJpeg( int index );

    // special case: custom function to replace ice_readCameraData
    void orca_readCameraData( Ice::InputStreamPtr iceInputStreamPtr , int index);

    // use opencv to convert to jpg

    // this is a counter of instaces of this type, used for registered interface name
    static int _counter;

    // from Replayer
    virtual void openLogFile();

    */

};

} // namespace
#endif
