#ifndef HYDROPUBLISH_QGRAPHICS2DPUBLISHER_H
#define HYDROPUBLISH_QGRAPHICS2DPUBLISHER_H

#include <boost/shared_ptr.hpp>
#include <QPicture>

namespace hydropublish {

//
// @brief Allows publishing of 2d graphics.
//
// @author Alex Brooks
//
class QGraphics2dPublisher
{
public: 
    virtual ~QGraphics2dPublisher() {}

    virtual void init()=0;
    virtual void localSetAndSend( const QPicture &picture )=0;

private: 

};
typedef boost::shared_ptr<QGraphics2dPublisher> QGraphics2dPublisherPtr;

// Does nothing
class DummyQGraphics2dPublisher : public QGraphics2dPublisher {
public:
    void init() {}
    void localSetAndSend( const QPicture &picture ) {}
};

//
// @brief Allows pulishers of unknown concrete type to be created
//
// @author Alex Brooks
//
class QGraphics2dPublisherFactory
{
public:
    virtual ~QGraphics2dPublisherFactory() {}

    virtual QGraphics2dPublisherPtr createPublisher( const std::string &name )=0;

private:
};
typedef boost::shared_ptr<QGraphics2dPublisherFactory> QGraphics2dPublisherFactoryPtr;

// Creates dummy publishers
class DummyQGraphics2dPublisherFactory : public QGraphics2dPublisherFactory {
public:
    QGraphics2dPublisherPtr createPublisher( const std::string &name )
        { return QGraphics2dPublisherPtr(new DummyQGraphics2dPublisher); }    
};

}

#endif
