/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2008 Tom Burdick, Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_IMAGEVIEW_QT_VIEWWIDGET_H
#define ORCA_IMAGEVIEW_QT_VIEWWIDGET_H

#include <hydroglu/gltexture.h>

#include <QTime>
#include <QGLWidget>


#include <orca/image.h>
#include <orcaice/ptrbuffer.h>

namespace imageviewqt
{
class ImageQueue;

class ViewWidget : public QGLWidget
{
Q_OBJECT

public:
    ViewWidget(ImageQueue* imageQueue, QWidget* parent=0);
    ~ViewWidget();


public slots:
    void flipX(bool);
    void flipY(bool);
    void fixedSize(bool);
    void zoom(double);

signals:
    void delayChanged(double); //! average latency of the images 
    void fpsChanged(double); //! average number of frames per second
    void widthChanged(int); //! width of the images
    void heightChanged(int); //! height of the images
    void formatChanged(QString); //! format of the images
    void zoomChanged(double);

protected:
    virtual void initializeGL();
    virtual void resizeGL(int width, int height);
    virtual void paintGL();

private:
    QTime timer_;

    void initializeTexture(uint32_t width, uint32_t height, std::string format);
    void updateTexture();

    void initializeVertices();
    void updateVertices();

    GLenum formatFromString(std::string format);

    // view state
    bool flipX_;
    bool flipY_;
    bool fixedSize_;
    double zoom_;

    // a shared buffer of images
    ImageQueue* imageQueue_;
   
    // the texture state
    hydroglu::GLTexture* texture_;
    bool textureInitialized_;
    QRectF textureVertices_;
    QRectF viewVertices_;
    
};

}

#endif

