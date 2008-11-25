#ifndef IMAGE_VIEW_IMAGEQUEUE_H
#define IMAGE_VIEW_IMAGEQUEUE_H

#include <orca/image.h>
#include <gbxsickacfr/gbxiceutilacfr/buffer.h>

#include <QObject>

namespace imageview
{

class ImageQueue : public QObject
{

Q_OBJECT

public:
    ImageQueue(uint32_t size, QObject *parent=0)
        : QObject(parent),
          imageQueue_(size, gbxiceutilacfr::BufferTypeCircular)
    {
    }
    
    void push(orca::ImageDataPtr image)
    {
        imageQueue_.push(image);
        Q_EMIT(imagePushed());
    }

    orca::ImageDataPtr pop()
    {
        orca::ImageDataPtr image;
        imageQueue_.getAndPop(image); 
        return image;
    }

    bool isEmpty()
    {
        return imageQueue_.isEmpty();
    }

signals:
    void imagePushed();

private:
    gbxiceutilacfr::Buffer<orca::ImageDataPtr> imageQueue_;
};

}

#endif
