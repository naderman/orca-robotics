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

class MainWindow : public QGLWidget
{
public:
    MainWindow() {}

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
        }
    void paintGL()
        {
            cout<<"TRACE(worldview3d.cpp): ==================== paintGL() ====================" << endl;

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
            glColor3f( 0.0, 1.0, 0.0 );
            glBegin(GL_QUADS);
            glNormal3f( 0.0, 0.0, 1.0);
            const double infty = 1000;
            glVertex3f(-infty,-infty,0);
            glVertex3f(-infty,infty,0);
            glVertex3f(infty,infty,0);
            glVertex3f(infty,-infty,0);
            glEnd();

//             glBegin (GL_TRIANGLES);
//             glColor3f (1.0, 0.0, 0.0);
//             glVertex2f (5.0, 5.0);
//             glColor3f (0.0, 1.0, 0.0);
//             glVertex2f (25.0, 5.0);
//             glColor3f (0.0, 0.0, 1.0);
//             glVertex2f (5.0, 25.0);
//             glEnd ();

            GridElement gridElement;
            gridElement.paint( this );

            glFlush();
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

    // Handles viewpoint
    orcaview3d::ViewHandler viewHandler_;
};

int main()
{
    // Set up QT stuff
    char **v = 0;
    int c = 0;
    QApplication qapp(c,v);

    MainWindow mainWin;
    mainWin.show();
    
    // note: this does not return!
    qapp.exec();

    return 0;
}
