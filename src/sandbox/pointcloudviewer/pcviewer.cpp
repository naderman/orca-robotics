#include "pcviewer.h"
#include <math.h>


using namespace orca;
using namespace std;

PointCloudViewer::~PointCloudViewer()
{}

void PointCloudViewer::loadFakeOgMap()
{
  // Read OG from PGM file
  // HACK: This routine can ONLY read in the files that were written above, not
  // general PGM files (in particular it can't handle comments).
  // Adapted from code written by Raymond Sheh.
  ogMap = new OgMapData();
  ogMap->origin.p.x = 100;
  ogMap->origin.p.y = 100;
  ogMap->origin.o = M_PI/4;
  ogMap->metresPerCellX = 4.0;
  ogMap->metresPerCellY = 4.0;

  // Could equally have done this with the orcamapload library functions, but the values used there are handled differently.
  FILE * pgmFile = fopen("mapData.pgm", "r");
  if (pgmFile == NULL)
  {
    fprintf(stderr, "ERROR, could not open file for reading.\n");
  }
  // Read the header.
  int readMaxNum;
  if (fscanf(pgmFile, "P2 %d %d %d", &(ogMap->numCellsX), &(ogMap->numCellsY), &readMaxNum) != 3)
  {
    fprintf(stderr, "ERROR, could not read header\n");
  }
  ogMap->data.clear();
  for (int i = 0; i < ogMap->numCellsX * ogMap->numCellsY; i ++)
  {
    int readNum;
    if (fscanf(pgmFile, " %d ", &readNum) != 1)
    {
      fprintf(stderr, "WARNING, read failed at element %d. Aborting read.\n", i);
      break;
    }
    ogMap->data.push_back(readNum);
  }
  fclose(pgmFile);
  freshOG = true;
}

PointCloudViewer::PointCloudViewer(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::AlphaChannel), parent),
      zoomFactor(1000),
      xOffset(0),
      yOffset(0),
      zOffset(0),
      yaw(0),
      pitch(0),
      fudgeFactor(3),
      showOGs(true),
      showSnaps(true),
      showLabels(true),
      showGrids(true),
      showRobots(true),
      showPointclouds(true),
      showPatchBorders(true),
      pointSize(3)
{
  setFocusPolicy(Qt::StrongFocus);
  makeCurrent();
  glGenTextures(1, &texId);
  loadFakeOgMap();
}

//void PointCloudViewer::setMapManager( QTMapDataInterface *in_mapManager){
//    //qDebug("set map manager");
//    mapManager = in_mapManager;
//    connect(mapManager, SIGNAL(patchChanged()), this, SLOT(update()));
//}

QSize PointCloudViewer::sizeHint()
{
  return QSize(640,480);
}

QSize PointCloudViewer::minimumSizeHint()
{
  return QSize(320,240);
}

void PointCloudViewer::initializeGL()
{
  glClearColor(0.70f, 0.7f, 0.7f, 1.0f);
  // Some stupid crap for QT
  renderText(0,0,0,"");
  pcList = glGenLists(1);
  ogList = glGenLists(1);
  cerr << "Display list is: " <<  pcList << endl;
}




void PointCloudViewer::resizeGL(int w, int h)
{
  screenWidth = w;
  screenHeight = h;
  aspectRatio = ((float) w)/((float) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  //qDebug("Aspect ratio set to %f", aspectRatio);
  //glOrtho(-aspectRatio, aspectRatio, -1, 1, -1000, 1000);
  gluPerspective(60, aspectRatio, 1,1000);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0,0,-2);
  glViewport(0,0,w,h);
  updateGL();
}

void PointCloudViewer::paintGL()
{
  //qDebug("paintGL on mapview called");
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_POINT_SMOOTH);
  glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_POLYGON_SMOOTH);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glScalef(1/zoomFactor, 1/zoomFactor, 1/zoomFactor);
  glColor4f(0,0,0,1);
  glBegin(GL_LINES);
  glVertex2f(zoomFactor*aspectRatio*0.90-1, -0.9*zoomFactor);
  glVertex2f(zoomFactor*aspectRatio*0.90, -0.9*zoomFactor);
  glEnd();
  renderText(zoomFactor*aspectRatio*0.90-1, -0.9*zoomFactor, 0, "scale: 1 m");
  glRotatef(pitch,1,0,0);
  glRotatef(yaw,0,0,1);

  glTranslatef(xOffset, yOffset, zOffset);

  if(showGrids)
  {
    for(int i=-(int) zoomFactor*3; i < (int) zoomFactor*3; i+=100)
    {
      glBegin(GL_LINES);
      if(i==0)
      {
        glColor4f(0,0,0,0.5);
      }
      else
      {
        glColor4f(0.5,0.5,0.5,0.5);
      }
      glVertex3f(-zoomFactor*3, i, 0);
      glVertex3f(zoomFactor*3, i, 0);
      glVertex3f(i,-zoomFactor*3, 0);
      glVertex3f(i, zoomFactor*3, 0);
      glEnd();
    }
  }
  //Ok, now let's draw dem maps
  if(showOGs)
  {
    renderOgMap();
  }
  if(showPointclouds)
  {
    glColor4f(0,0,0,1.0);
    glPushMatrix();
    glRotatef(90,1,0,0);
    renderPointCloud();
    glPopMatrix();
  }
  glDisable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_POINT_SMOOTH);
  glDisable(GL_LINE_SMOOTH);
  glDisable(GL_POLYGON_SMOOTH);
  glPopMatrix();
}

void PointCloudViewer::renderPointCloud()
{
  //glScalef(0.01,0.01,0.01);
  glDisable(GL_POINT_SMOOTH);
  glPointSize(pointSize);

  if(freshPC)
  {
    glNewList(pcList, GL_COMPILE_AND_EXECUTE);
    glBegin(GL_POINTS);
    for(int l=0; l < cloud->points.size()/6; l++)
    {
      glColor4f(cloud->points[l*6+3], cloud->points[l*6+4], cloud->points[l*6+5], 0.3);
      glVertex3f(cloud->points[l*6], cloud->points[l*6+1], cloud->points[l*6+2]);
      //qDebug("z value is: %f", cloud->points[l*6+2]);
      //glVertex3f(cloud->points[l*6]/cloud->points[l*6+2]*-100, cloud->points[l*6+1]/cloud->points[l*6+2]*-100, -100);

    }
    glEnd();
    glEndList();
    freshPC=false;
  }
  else
  {
    glCallList(pcList);
  }
  glPointSize(1.0);
}

void PointCloudViewer::renderOgMap()
{
  int textWidth;
  int textHeight;
  float textWFrac;
  float textHFrac;

  if(freshOG)
  {
    // Find the next biggest power of two.
    textWidth = (int) powl(2, ceill(log(ogMap->numCellsX)/log(2)));
    textHeight = (int)  powl(2, ceill(log(ogMap->numCellsY)/log(2)));
    textWFrac = ((float) ogMap->numCellsX)/textWidth;
    textHFrac = ((float) ogMap->numCellsY)/textHeight;
    // qDebug("Width and height are %d and %d", textWidth, textHeight);
    // It's a pity that we can't assume support for non power of two dimensions textures
    // otherwise the above code could be simplified code below could be used.
    // textWidth = ogMap->numCellsX;
    // textHeight = ogMap->numCellsY;
    // textWFrac = 1;
    // textHFrac = 1;

    unsigned char imgData[textWidth*textHeight*4];
    int min=255;
    int max=0;
    float mean=0;
    for(int i=0; i < ogMap->numCellsY; i++)
    {
      for(int j=0; j < ogMap->numCellsX; j++)
      {
        // qDebug("Pixel %d has value %d", k, (unsigned char) ogMap->data[k]);
        unsigned char val = (unsigned char) ogMap->data[i*ogMap->numCellsX+j];
        if(val < min) min=val;
        if(val > max) max=val;
        mean += val;
        if(val > 90)
        {
          // Interpreted as a solid wall.
          imgData[(i*textWidth+j)*4] = 0;
          imgData[(i*textWidth+j)*4+1] =0;
          imgData[(i*textWidth+j)*4+2] = 255;
          imgData[(i*textWidth+j)*4+3] = 255;
        }
        else if(val < 70)
        {
          imgData[(i*textWidth+j)*4] = 255;
          imgData[(i*textWidth+j)*4+1] =255;
          imgData[(i*textWidth+j)*4+2] = 255;
          imgData[(i*textWidth+j)*4+3] = 50;
        }
        else
        {
          imgData[(i*textWidth+j)*4] = 0;
          imgData[(i*textWidth+j)*4+1] = 0;
          imgData[(i*textWidth+j)*4+2] = 0;
          imgData[(i*textWidth+j)*4+3] = 0;
        }
      }
    }
    glNewList(ogList, GL_COMPILE_AND_EXECUTE);
    // qDebug("Map values %d %d %f" , min, max, mean/(ogMap->numCellsX*ogMap->numCellsY));
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, textWidth, textHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgData);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glEnable(GL_TEXTURE_2D);
    glPushMatrix();
    glTranslatef(ogMap->origin.p.x, ogMap->origin.p.y,0);
    glRotatef(ogMap->origin.o*180.0/M_PI,0,0,1);
    glScalef(1,-1,1);
    glTranslatef(-ogMap->numCellsX*ogMap->metresPerCellX/2.0,-ogMap->numCellsY*ogMap->metresPerCellY/2,0);
    glShadeModel(GL_FLAT);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0,0.0);
    glVertex2f(0.0,0.0);
    glTexCoord2f(0.0,textHFrac);
    glVertex2f(0.0, ogMap->numCellsY*ogMap->metresPerCellY);
    glTexCoord2f(textWFrac, textHFrac);
    glVertex2f(ogMap->numCellsX*ogMap->metresPerCellX,ogMap->numCellsY*ogMap->metresPerCellY);
    glTexCoord2f(textWFrac,0.0);
    glVertex2f(ogMap->numCellsX*ogMap->metresPerCellX,0.0);
    glEnd();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    glEndList();
    freshOG = false;
  }
  else
  {
    glCallList(ogList);
  }

  // We now draw a surrounding rectangle so people know where the edge of the OG map is.
  if(showPatchBorders)
  {
    glPushMatrix();
    glTranslatef(ogMap->origin.p.x, ogMap->origin.p.y,0);
    glRotatef(ogMap->origin.o*180.0/M_PI,0,0,1);
    glScalef(1,-1,1);
    glTranslatef(-ogMap->numCellsX*ogMap->metresPerCellX/2.0,-ogMap->numCellsY*ogMap->metresPerCellY/2,0);
    glColor4f(0,0,1,0.5);
    glBegin(GL_LINE_LOOP);
    glVertex2f(0,0);
    glVertex2f(0.0, ogMap->numCellsY*ogMap->metresPerCellY);
    glVertex2f(ogMap->numCellsX*ogMap->metresPerCellX,ogMap->numCellsY*ogMap->metresPerCellY);
    glVertex2f(ogMap->numCellsX*ogMap->metresPerCellX,0.0);
    glEnd();
    glPopMatrix();
  }

}

void PointCloudViewer::setPointCloud(PointCloudPtr pointCloud)
{
  cloud = pointCloud;
  freshPC = true;
  //makeCurrent();
  //glNewList(pcList, GL_COMPILE);
  //glScalef(0.01,0.01,0.01);
  //     glDisable(GL_POINT_SMOOTH);
  //     glPointSize(screenWidth*0.01/zoomFactor);
  //     glBegin(GL_POINTS);
  //     for(int l=0; l < cloud->points.size()/6; l++){
  // 	glColor4f(cloud->points[l*6+3], cloud->points[l*6+4], cloud->points[l*6+5], 0.3);
  // 	glVertex3f(cloud->points[l*6], cloud->points[l*6+1], cloud->points[l*6+2]);
  // 		//qDebug("z value is: %f", cloud->points[l*6+2]);
  // 		//glVertex3f(cloud->points[l*6]/cloud->points[l*6+2]*-100, cloud->points[l*6+1]/cloud->points[l*6+2]*-100, -100);
  //
  //     }
  //     glEnd();
  //     glPointSize(1.0);
  //     glEndList();
}

void PointCloudViewer::keyPressEvent(QKeyEvent *e)
{
  if(e->key() == Qt::Key_W)
  {
    yOffset += 0.1*zoomFactor;
  }
  else if(e->key() == Qt::Key_S)
  {
    yOffset -= 0.1*zoomFactor;
  }
  else if(e->key() == Qt::Key_A)
  {
    xOffset -= 0.1*zoomFactor;
  }
  else if(e->key() == Qt::Key_D)
  {
    xOffset += 0.1*zoomFactor;
  }
  else if(e->key() == Qt::Key_P)
  {
    pointSize += 1;
  }
  else if(e->key() == Qt::Key_O)
  {
    pointSize -= 1;
    if(pointSize < 1) pointSize = 1;
  }
  else if(e->key() == Qt::Key_BracketLeft)
  {
    zoomFactor *= 1.1;
  }
  else if(e->key() == Qt::Key_BracketRight)
  {
    zoomFactor /= 1.1;
  }
  else if(e->key() == Qt::Key_Left)
  {
    yaw += 5;

  }
  else if(e->key() == Qt::Key_Right)
  {
    yaw -= 5;
  }
  else if(e->key() == Qt::Key_Up)
  {
    pitch += 5;
    if(pitch > 90) pitch = 90;
  }
  else if(e->key() == Qt::Key_Down)
  {
    pitch -= 5;
    if(pitch < -90) pitch = -90;
  }
  else if(e->key() == Qt::Key_R)
  {
    zoomFactor=1000;
    xOffset= yOffset=zOffset=yaw=pitch=0;
  }
  else if(e->text() == "=")
  {
    fudgeFactor *=1.25;
    qDebug("Fudge factor set to %f", fudgeFactor);
  }
  else if(e->text()=="-")
  {
    fudgeFactor /=1.25;
    qDebug("Fudge factor set to %f", fudgeFactor);
  }
  else if(e->text() == "0")
  {
    fudgeFactor=3;
    qDebug("Fudge factor set to %f", fudgeFactor);
  }
  else if(e->key() == Qt::Key_G)
  {
    showOGs = !showOGs;
  }
  else if(e->key() == Qt::Key_C)
  {
    showPointclouds = !showPointclouds;
  }
  else if(e->key() == Qt::Key_L)
  {
    showGrids = !showGrids;
  }
  else if(e->key() == Qt::Key_B)
  {
    showPatchBorders = !showPatchBorders;
  }
  update();
}

void PointCloudViewer::focusInEvent(QFocusEvent *fe)
{
  makeCurrent();
  glClearColor(0.7f,0.7f,0.7f,1.0f);
  updateGL();
}

void PointCloudViewer::focusOutEvent(QFocusEvent *fe)
{
  makeCurrent();
  //glClearColor(0.3f,0.3f,0.3f,1.0f);
  updateGL();
}

QImage PointCloudViewer::captureMap()
{
  return grabFrameBuffer();
}
