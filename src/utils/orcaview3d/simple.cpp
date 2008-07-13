#include <iostream>
#include <orcaqgui3d/glutil.h>
#include <QGLWidget>
#include <QApplication>
#include "viewhandler.h"
#include <sstream>

using namespace std;

/*!
  @brief Paints a rectangular grid in the 3d display.
  @author Alex Brooks
*/  
class GridElement
{
public:

    GridElement() {}
	
    void paint( QGLWidget *p )
        {
            drawOrigin();
            drawGridAndLabels( p );
        }

private:

    void drawGridAndLabels( QGLWidget *p )
        {
            // Major line every N lines (others are minor)
            const int majorEveryN=2;
            const int numMajor=3;
            const float gridWidth=6; // [m]

            int gridLinesEachDirn=numMajor*majorEveryN;

            float w = gridWidth/2.0;

            for(int i=-gridLinesEachDirn; i <= gridLinesEachDirn; i++)
            {
                float d = gridWidth*(float)i/(float)(2*gridLinesEachDirn);
                if(i%(majorEveryN)==0)
                {
                    glColor4f(0,0,0,0.5);

                    bool isDisplayLabels = true;
                    if ( isDisplayLabels )
                    {
                        if ( i != 0 )
                        {
                            std::stringstream ss;
                            ss << (int)d;
                            p->renderText( d, 0, 0, ss.str().c_str() );
                            p->renderText( 0, d, 0, ss.str().c_str() );
                        }
                    }
                }
                else 
                {
                    glColor4f(0.5,0.5,0.5,0.5);
                }

                glBegin(GL_LINES);
                glVertex3f(-w, d, 0);
                glVertex3f(w,  d, 0);
                glVertex3f(d, -w, 0);
                glVertex3f(d,  w, 0);
                glEnd();
            }
        }
    void drawOrigin()
        {
            // Draw axes on origin
            const float scaleAxes=5.0;
            const float d = 0.2*scaleAxes;
            glBegin(GL_LINES);
            glColor4f(0,0,1,1);
            glVertex3f(0,0,0);
            glVertex3f(d,0,0);
            glVertex3f(0,0,0);
            glVertex3f(0,d,0);
            glVertex3f(0,0,0);
            glVertex3f(0,0,d);

            const float p=0.1*scaleAxes;
            // x
            glVertex3f(d+p  ,p/2.0,0);
            glVertex3f(d+2*p,-p/2.0,0);
            glVertex3f(d+p  ,-p/2.0,0);
            glVertex3f(d+2*p,p/2.0,0);

            // y
            glVertex3f(-p/2.0,d+p,0);
            glVertex3f(p/2.0 ,d+2*p,0);
            glVertex3f(-p/2.0,d+2*p,0);
            glVertex3f(0     ,d+1.5*p,0);

            // z
            glVertex3f(-p/2.0,0,d+p);
            glVertex3f(p/2.0, 0,d+p);
            glVertex3f(-p/2.0,0,d+p);
            glVertex3f(p/2.0, 0,d+2*p);
            glVertex3f(-p/2.0,0,d+2*p);
            glVertex3f(p/2.0, 0,d+2*p);

            glEnd();
        }
};

class Billboard {
private:
    static const int IMG_WIDTH=64;
public:

    void init()
        {
            // Draw the chess-board
            orcaqgui3d::glutil::makeCheckImage( checkImage_ );

            // Says how to read the texture in the next call
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

            // Create the texture map
            glTexImage2D(GL_TEXTURE_2D, 0, 3, IMG_WIDTH, 
                         IMG_WIDTH, 0, GL_RGB, GL_UNSIGNED_BYTE, 
                         &checkImage_[0][0][0]);
        }

    void paint()
        {
            glEnable( GL_TEXTURE_2D );
            {
                ////////////////////////////////////////
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                ////////////////////////////////////////
                glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

                const float w = 3.0;

                glBegin(GL_QUADS);
                glNormal3f( 1.0, 0.0, 0.0);
                glColor3f( 0.0, 1.0, 1.0 );
                glTexCoord2f(0.0, 0.0); glVertex3f(0,w/2.0,0);
                glTexCoord2f(0.0, 1.0); glVertex3f(0,w/2.0,w);
                glTexCoord2f(1.0, 1.0); glVertex3f(0,-w/2.0,w);
                glTexCoord2f(1.0, 0.0); glVertex3f(0,-w/2.0,0);
                glEnd();
            }
            glDisable( GL_TEXTURE_2D );
        }

private:

    GLubyte checkImage_[IMG_WIDTH][IMG_WIDTH][3];
};

class MainWindow : public QGLWidget
{
public:

    class Config {
    public:
        enum Quality {
            Nicest,
            Fastest,
        };

        Config( Quality pQuality )
            : quality(pQuality)
            {
                enableAntialiasing = (quality == Nicest);
                enableTexturing = (quality == Nicest);
            }

        // General quality
        Quality quality;
        bool    enableAntialiasing;
        bool    enableTexturing;
    };

    //////////////////////////////////////////////////////////////////////

    MainWindow( const Config &config ) 
        : config_(config)
        {}

    // from QGLWidget
    void initializeGL()
        {
            cout<<"TRACE(simple.cpp): init()" << endl;

            // Set the default (background) color
            glClearColor(0.70f, 0.7f, 0.7f, 0.0f);

            // Enable the depth buffer: to handle hidden surface removal
            glEnable(GL_DEPTH_TEST);

            // GL_SMOOTH is the default
            // glShadeModel( GL_SMOOTH );

            // Only load the texture-map once
            billboard_.init();
        }
    void paintGL()
        {
            cout<<"TRACE(worldview3d.cpp): ==================== paintGL() ====================" << endl;

            // Set config
            // (don't really need to do this on every draw, just when it's changed...)
            setQuality();

            // Start from a clean slate
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

            // Clear the viewing transformation
            glLoadIdentity();

            // Put the camera in position
            viewHandler_.applyViewingTransformation();
            //glScalef( 1.0, 0.3, 0.3 );
            //glTranslatef(0, 0, -5);    
            
//             glPushMatrix();
//             {
//                 glTranslatef( 0, 0, 3 );
//                 glColor3f( 1.0, 0.0, 0.0 );
//                 orcaqgui3d::glutil::drawBox( 1, 1, 1, true, true );
//             }
//             glPopMatrix();

            // Ground plane
            glBegin(GL_QUADS);
            glNormal3f( 0.0, 0.0, 1.0);
            const double infty = 1000;
            glColor3f( 0.0, 1.0, 0.0 );
            glVertex3f(-infty,-infty,0);
            glVertex3f(-infty,infty,0);
            glVertex3f(infty,infty,0);
            glVertex3f(infty,-infty,0);
            glEnd();


            // gradient-ed square
            glBegin(GL_QUADS);
            glNormal3f( 0.0, 0.0, 1.0);
            const double in = 3;
            const double height = 4;
            glColor3f( 0.0, 1.0, 1.0 );
            glVertex3f(-in,-in,height);
            glVertex3f(-in,in,height);
            glColor3f( 1.0, 0.0, 0.0 );
            glVertex3f(in,in,height);
            glColor3f( 1.0, 1.0, 0.0 );
            glVertex3f(in,-in,height);
            glEnd();

            // Grid
            glPushMatrix();
            {
                glTranslatef( 0, 0, 0.05 );
                GridElement gridElement;
                gridElement.paint( this );
            }
            glPopMatrix();

            // Another Grid
            glPushMatrix();
            {
                glTranslatef( 0, 10, 0.05 );
                GridElement gridElement;
                gridElement.paint( this );
            }
            glPopMatrix();

            // Billboard
            glPushMatrix();
            {
                glTranslatef( 4, 0, 0 );
                billboard_.paint();
            }
            glPopMatrix();

            orcaqgui3d::glutil::checkGLError();
        }
    void resizeGL(int w, int h)
        {
            cout<<"TRACE(worldview3d.cpp): resizeGL()" << endl;

            // Set the projection matrix
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            // sets the perspective:
            //  (left,right,bottom,top,near,far)
            glFrustum (-1.0, 1.0, -1.0, 1.0, 1.0, 200.0); 

            // back to modelview matrix
            glMatrixMode (GL_MODELVIEW);
            // define the viewport
            glViewport (0, 0, w, h);
        }

    void mousePressEvent( QMouseEvent *e )
        { viewHandler_.mousePressEvent(e); }
    void mouseMoveEvent( QMouseEvent *e )
        {
            const bool needUpdate = viewHandler_.mouseMoveEvent(e);
            if ( needUpdate )
                updateGL();
        }
    void keyPressEvent(QKeyEvent *e)
        {
            const bool needUpdate = viewHandler_.keyPressEvent(e);
            if ( needUpdate )
                updateGL();
            else
                QGLWidget::keyPressEvent(e);
        }            

private:

    void setQuality()
        {
            // TODO: this is bad: in fact we can turn quality params
            // on and off per-element.
            return;

            // Set all the hints
            GLenum hint = GL_DONT_CARE;
            switch ( config_.quality )
            {
            case Config::Nicest:
            {
                cout<<"TRACE(simple.cpp): Setting quality to nicest" << endl;
                hint = GL_NICEST; break;
            }
            case Config::Fastest:
            {
                cout<<"TRACE(simple.cpp): Setting quality to fastest" << endl;
                hint = GL_FASTEST; break;
            }
            }
            glHint( GL_POINT_SMOOTH_HINT, hint );
            glHint( GL_LINE_SMOOTH_HINT, hint );
            glHint( GL_POLYGON_SMOOTH_HINT, hint );
            glHint( GL_FOG_HINT, hint );
            glHint( GL_PERSPECTIVE_CORRECTION_HINT, hint );            

            // Antialiasing
            if ( config_.enableAntialiasing )
            {
                glEnable( GL_POINT_SMOOTH );
                glEnable( GL_LINE_SMOOTH );
                glEnable( GL_BLEND );
                glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
            }
            else
            {
                glDisable( GL_POINT_SMOOTH );
                glDisable( GL_LINE_SMOOTH );
                glDisable( GL_BLEND );
            }

            // Texturing (2D only)
            if ( config_.enableTexturing )
                glEnable( GL_TEXTURE_2D );
            else
                glDisable( GL_TEXTURE_2D );
        }

    Config config_;

    // Handles viewpoint
    orcaview3d::ViewHandler viewHandler_;


    Billboard billboard_;
};

int main()
{
    // Set up QT stuff
    char **v = 0;
    int c = 0;
    QApplication qapp(c,v);

    MainWindow::Config config( MainWindow::Config::Nicest );

    MainWindow mainWin( config );
    mainWin.show();
    
    // note: this does not return!
    qapp.exec();

    return 0;
}
