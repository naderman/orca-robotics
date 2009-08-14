#include <GL/glew.h>

#include <cstdlib>
#include <ctime>

#include <hydroglu/hydroglu.h>

#include <QDebug>
#include <QApplication>
#include <QGLWidget>


const char vertSource[]=
"void main()\n"
"{\n"
"   gl_Position = ftransform();\n"
"}\n"
"\n";

const char fragSource[]=
"uniform vec4 color;\n"
"void main ()\n"
"{\n"
"        gl_FragColor = color;\n"
"}\n"
"\n";


class GLTest : public QGLWidget
{
public:
    GLTest( QWidget* parent = 0 )
    : QGLWidget(parent)
    {
        setAttribute(Qt::WA_QuitOnClose);
        iter_ = 0;
        startTimer( 5 );

    }

protected:
    virtual void timerEvent(QTimerEvent * event)
    {
        updateGL();
    }

    virtual void updateGL()
    {
        iter_++;
        if(iter_ > 200)
        {
            close(); 
        }

        QGLWidget::updateGL();
    }

    virtual void initializeGL()
    {
        GLenum err = glewInit();
        if( GLEW_OK != err )
        {
            throw gbxutilacfr::Exception(ERROR_INFO, "GlEW failed to initialize");
        }

        if( !GLEW_VERSION_2_0 )
        {
            throw gbxutilacfr::Exception(ERROR_INFO, "OpenGL version too old, need 2.0");
        }

        // drawing options
        glClearColor(0.0, 0.0, 0.0, 0.0);

        program_ = new hydroglu::GLProgram("TestProgram");
        hydroglu::GLVertexShaderPtr vertShader = 
            hydroglu::GLVertexShader::fromString(vertSource, "Vertex");
        hydroglu::GLFragmentShaderPtr fragShader =
            hydroglu::GLFragmentShader::fromString(fragSource, "Frag");
        program_->addShader(vertShader);
        program_->addShader(fragShader);
        program_->link();
        program_->enable();
        program_->setUniform("color", 0.8f, 0.1f, 0.3f, 0.0f);
        program_->disable();
    }

    virtual void resizeGL(int width, int height)
    {
        // matrices
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glViewport(0, 0, width, height);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0.0, width, 0.0, height);
    }

    virtual void paintGL()
    {
        int start = iter_;
        program_->enable();

        program_->setUniform("color", (rand() % 256)/256.0f, (rand() % 256)/256.0f, (rand() % 256)/256.0f, 0.0);

        glClear(GL_COLOR_BUFFER_BIT);

        glBegin(GL_QUADS);
        
        glVertex2i( start, start );
        
        glVertex2i( start, start+50 );
        
        glVertex2i( start+50, start+50 );
        
        glVertex2i( start+50, start );
        
        glEnd();
        
        program_->disable();
    }

private:
    uint32_t iter_;
    hydroglu::GLProgramPtr program_;

};


int main(int argc, char* argv[])
{
    srand( time(NULL) );

    QApplication app(argc, argv);

    GLTest widget;

    widget.show();
    
    return app.exec();
}



