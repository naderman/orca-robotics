#ifndef  VELOCITYCONTROL_DISPLAY_WIDGET_H
#define  VELOCITYCONTROL_DISPLAY_WIDGET_H

#include <QObject>
#include <orcateleop/display.h>

class QTextEdit;
class QSplitter;

namespace orcaqgui2d {

//
// Class which sets up a text window displaying feedback of teleop commands
//
// @author Tobias Kaupp
//
class VelocityControlDisplay : public QObject,
                               public orcateleop::Display
{
    Q_OBJECT
            
    public:
        VelocityControlDisplay( const std::string      &platformName, 
                                QSplitter              *spaceBottomRight=0 );
                
        virtual ~VelocityControlDisplay();

        //
        // Inherited from orcateleop::Display
        // These fcts will be called from the network thread, emit signals whose slots run in the main GUI thread
        // Both of these operations are thread-safe
        //
        
        virtual void sentNewVelocityCommand( double vx, double vy, double w, bool vxLimit=false, bool vyLimit=false, bool wLimit=false );

        virtual void sentNewBicycleCommand( double speed, double steerAngle, bool speedLimit=false, bool steerAngleLimit=false ) {};

        virtual void sentRepeatCommand();

        virtual void failedToSendCommand( const std::string &errorMsg );
        
        
    private:
        QTextEdit *display_;
        double vx_;
        double vy_;
        double w_;
        
    signals:
        void newStringAvailable( const QString & );
        
};

}

#endif
