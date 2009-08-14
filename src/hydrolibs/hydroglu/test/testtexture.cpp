#include <GL/glew.h>

#include <cstdlib>
#include <ctime>

#include <hydroglu/hydroglu.h>

#include <QDebug>
#include <QApplication>
#include <QGLWidget>
#include <QImage>

const char vertSource[]=
"void main()\n"
"{\n"
"   gl_TexCoord[0] = gl_MultiTexCoord0;\n"
"   gl_Position = ftransform();\n"
"}\n"
"\n";

const char fragSource[]=
"uniform sampler2D tex;\n"
"void main ()\n"
"{\n"
"        gl_FragColor = texture2D(tex, gl_TexCoord[0].st);\n"
"}\n"
"\n";


class GLTest : public QGLWidget
{
public:
    GLTest( QImage image, QWidget* parent = 0 )
    : QGLWidget(parent)
    {
        image_ = QGLWidget::convertToGLFormat( image );
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
    
        glEnable(GL_TEXTURE_2D);
        
        texture_ = new hydroglu::GLTexture("Test Texture", image_.width(), image_.height());
        texture_->upload(image_.bits(), image_.numBytes(), GL_RGBA);
        
        program_ = new hydroglu::GLProgram("Test Program");
        hydroglu::GLVertexShaderPtr vertShader = 
            hydroglu::GLVertexShader::fromString(vertSource, "Vertex");
        hydroglu::GLFragmentShaderPtr fragShader =
            hydroglu::GLFragmentShader::fromString(fragSource, "Frag");
        program_->addShader(vertShader);
        program_->addShader(fragShader);
        program_->link();
        program_->enable();
        program_->setUniform("tex", 0);
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

        glClear(GL_COLOR_BUFFER_BIT);
        
        glActiveTexture(GL_TEXTURE0);
        texture_->pushBind();

        glBegin(GL_QUADS);
        
        glTexCoord2f( TEXCOORD_BL(texture_) ); 
        glVertex2i( start, start );

        glTexCoord2f( TEXCOORD_BR(texture_) );
        glVertex2i( start+100, start );


        glTexCoord2f( TEXCOORD_TR(texture_) );
        glVertex2i( start+100, start+100 );

        glTexCoord2f( TEXCOORD_TL(texture_) );
        glVertex2i( start, start+100 );

        glEnd();

        texture_->popBind();
        
        program_->disable();
    }


private:
    uint32_t iter_;
    hydroglu::GLProgramPtr program_;
    hydroglu::GLTexturePtr texture_;
    QImage image_;
};


int main(int argc, char* argv[])
{
    srand( time(NULL) );

    QApplication app(argc, argv);
    
    if(argc < 2)
    {
        qDebug() << "no image name given";
        return -1;
    }

    QImage image(argv[1]);
    if( image.isNull() )
    {
        qDebug() << "no valid image given";
        return -1;
    }

    GLTest widget(image);
    widget.show();
    
    return app.exec();
}



