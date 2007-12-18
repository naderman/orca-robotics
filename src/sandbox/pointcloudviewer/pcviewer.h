#ifndef ORCA_POINT_CLOUD_VIEWER_H
#define ORCA_POINT_CLOUD_VIEWER_H

#include <orca/pointcloud.h>
#include <orca/ogmap.h>

#include <QtOpenGL>
#include <GL/glut.h>

class PointCloudViewer : public QGLWidget
{
Q_OBJECT
  public:
        PointCloudViewer(QWidget *parent=0);
	~PointCloudViewer();
        void initializeGL();
        void paintGL();
        void resizeGL(int w, int h);
        QSize sizeHint();
        QSize minimumSizeHint();
	void keyPressEvent(QKeyEvent *e);
	void focusInEvent(QFocusEvent *fe); 
	void focusOutEvent(QFocusEvent *fe); 
	QImage captureMap(); 
	void setPointCloud( const orca::PointCloudData& pointCloud ); 
  private: 
        void loadFakeOgMap(); 
	void renderPointCloud();
        void renderOgMap(); 
	int screenWidth;
        int screenHeight;  
	float zoomFactor; 
	float xOffset, yOffset, zOffset; 
	float yaw, pitch; 
	float aspectRatio; 
	float fudgeFactor; 
	//int displayList; 
	bool showOGs;
	bool showSnaps;
	bool showLabels; 
	bool showGrids; 
	bool showRobots; 
	bool showPointclouds; 
	bool showPatchBorders;
	GLunsigned int texId;
	GLunsigned int pcList;
        GLunsigned int ogList; 
	orca::PointCloudPtr cloud; 
	bool freshPC; 
        bool freshOG; 
	float pointSize; 
        orca::OgMapData ogMap; 
};
   
#endif
