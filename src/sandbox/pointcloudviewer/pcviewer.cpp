#include "pcviewer.h"
using namespace orca; 
using namespace std; 

PointCloudViewer::~PointCloudViewer(){

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
} 

//void PointCloudViewer::setMapManager( QTMapDataInterface *in_mapManager){
//    //qDebug("set map manager");
//    mapManager = in_mapManager; 
//    connect(mapManager, SIGNAL(patchChanged()), this, SLOT(update())); 
//}

QSize PointCloudViewer::sizeHint(){
    return QSize(640,480);   
}

QSize PointCloudViewer::minimumSizeHint(){
    return QSize(320,240);   
}

void PointCloudViewer::initializeGL(){
    glClearColor(0.70f, 0.7f, 0.7f, 1.0f);
    // Some stupid crap for QT
    renderText(0,0,0,""); 
    displayList = glGenLists(1);
    cerr << "Display list is: " <<  displayList << endl; 
}




void PointCloudViewer::resizeGL(int w, int h){
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

void PointCloudViewer::paintGL(){
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

    if(showGrids){
	for(int i=-(int) zoomFactor*3; i < (int) zoomFactor*3; i+=100){
	    glBegin(GL_LINES);
	    if(i==0){
		glColor4f(0,0,0,0.5);  
	    }
	    else {
		glColor4f(0.5,0.5,0.5,0.5); 
	    }
	    glVertex3f(-zoomFactor*3, i, 0); 
	    glVertex3f(zoomFactor*3, i, 0); 
	    glVertex3f(i,-zoomFactor*3, 0); 
	    glVertex3f(i, zoomFactor*3, 0); 
	    glEnd(); 
	}
    }
    //Ok, now let's draw dem maps.
    glColor4f(0,0,0,1.0);
    glPushMatrix();  
    glRotatef(90,1,0,0);
    renderPointCloud(); 
    glPopMatrix(); 
    
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_POINT_SMOOTH); 
    glDisable(GL_LINE_SMOOTH); 
    glDisable(GL_POLYGON_SMOOTH);
    glPopMatrix();
}

void PointCloudViewer::renderPointCloud(){ 
    //glScalef(0.01,0.01,0.01);
   glDisable(GL_POINT_SMOOTH);
   glPointSize(pointSize);
     
   if(freshPC){
    glNewList(displayList, GL_COMPILE_AND_EXECUTE);
    glBegin(GL_POINTS);
    for(int l=0; l < cloud->points.size()/6; l++){
	glColor4f(cloud->points[l*6+3], cloud->points[l*6+4], cloud->points[l*6+5], 0.3); 
	glVertex3f(cloud->points[l*6], cloud->points[l*6+1], cloud->points[l*6+2]); 
		//qDebug("z value is: %f", cloud->points[l*6+2]);
		//glVertex3f(cloud->points[l*6]/cloud->points[l*6+2]*-100, cloud->points[l*6+1]/cloud->points[l*6+2]*-100, -100);
		
    }
    glEnd();
    glEndList();
    freshPC=false; 
   }
   else {
      glCallList(displayList);
    }
    glPointSize(1.0); 
}

void PointCloudViewer::setPointCloud(PointCloudPtr pointCloud){
    cloud = pointCloud;
    freshPC = true; 
    //makeCurrent();
    //glNewList(displayList, GL_COMPILE); 
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

void PointCloudViewer::keyPressEvent(QKeyEvent *e){
    if(e->key() == Qt::Key_W){
	yOffset += 0.1*zoomFactor;
    }
    else if(e->key() == Qt::Key_S){
	yOffset -= 0.1*zoomFactor;
    }
    else if(e->key() == Qt::Key_A){
	 xOffset -= 0.1*zoomFactor; 
    }
    else if(e->key() == Qt::Key_D){
	xOffset += 0.1*zoomFactor; 
    } 
    else if(e->key() == Qt::Key_P){
 	pointSize += 1; 
    }
    else if(e->key() == Qt::Key_O){
      pointSize -= 1; 
      if(pointSize < 1) pointSize = 1; 
    }
    else if(e->key() == Qt::Key_BracketLeft){
	zoomFactor *= 1.1; 
    }
    else if(e->key() == Qt::Key_BracketRight){
	zoomFactor /= 1.1; 
    }
    else if(e->key() == Qt::Key_Left){
	yaw += 5; 
	
    }
    else if(e->key() == Qt::Key_Right){
	    yaw -= 5;
 	}
    else if(e->key() == Qt::Key_Up){
	pitch += 5; 
	if(pitch > 90) pitch = 90; 
    }
    else if(e->key() == Qt::Key_Down){
	pitch -= 5; 
	if(pitch < -90) pitch = -90; 
    }
    else if(e->key() == Qt::Key_R){
	zoomFactor=1000;
	xOffset= yOffset=zOffset=yaw=pitch=0;
    }
    else if(e->text() == "="){
	fudgeFactor *=1.25;
	qDebug("Fudge factor set to %f", fudgeFactor); 
    }
    else if(e->text()=="-"){
	fudgeFactor /=1.25;
	qDebug("Fudge factor set to %f", fudgeFactor); 	
    }
    else if(e->text() == "0"){
	fudgeFactor=3;
	qDebug("Fudge factor set to %f", fudgeFactor); 	
    }
    
    
    update(); 
}

void PointCloudViewer::focusInEvent(QFocusEvent *fe){
    makeCurrent(); 
    glClearColor(0.7f,0.7f,0.7f,1.0f);   
    updateGL();
}

void PointCloudViewer::focusOutEvent(QFocusEvent *fe){
    makeCurrent();  
    //glClearColor(0.3f,0.3f,0.3f,1.0f);  
    updateGL(); 
}

QImage PointCloudViewer::captureMap(){
    return grabFrameBuffer();
}

