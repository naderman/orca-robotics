#ifndef ORCA_IMAGE_VIEW_VIEWWIDGET_H
#define ORCA_IMAGE_VIEW_VIEWWIDGET_H

#include <QGLWidget>

#include <orca/image.h>
#include <orcaice/ptrbuffer.h>

namespace orcaimageview
{

class ViewWidget : public QGLWidget
{
public:
    ViewWidget(orcaice::PtrBuffer<orca::ImageDataPtr>* imageQueue, QWidget* parent=0);
    ~ViewWidget();

protected:
    virtual void initializeGL();
    virtual void resizeGL(int width, int height);
    virtual void paintGL();

private:
    void initializeTexture(uint32_t width, uint32_t height, std::string format);
    void updateTexture();
    GLenum formatFromString(std::string format);

    // a shared linked list of images
    orcaice::PtrBuffer<orca::ImageDataPtr>* imageQueue_;
   
    // the texture state
    GLuint texture_;
    uint32_t textureWidth_;
    uint32_t textureHeight_;
    GLuint textureFormat_;
    bool textureInitialized_;

};

}

#endif

