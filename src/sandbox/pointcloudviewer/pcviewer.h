#ifndef ORCA2_POINT_CLOUD_VIEWER_H
#define ORCA2_POINT_CLOUD_VIEWER_H

#include <orca/pointcloud.h>

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
	void setPointCloud(orca::PointCloudPtr pointCloud); 
  private: 
	void renderPointCloud();
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
	GLuint texId;
	GLuint displayList;
	orca::PointCloudPtr cloud; 
	bool freshPC; 
	float pointSize; 
};
   
#endif
