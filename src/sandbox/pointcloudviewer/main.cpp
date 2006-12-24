#include "pointcloudconsumerI.h"
#include "pcviewer.h"
#include <orca/pointcloud.h>
#include <QApplication>
#include <QMainWindow>

using namespace pcviewer; 
using namespace std; 

int main(int argc, char **argv)
{
  QApplication a( argc, argv );
  //glutInit(&argc,argv); 

  std::string endpoint = "default -h localhost -p 10101 -z"; 
  PointCloudConsumerI *pcci = new PointCloudConsumerI(endpoint); 
  pcci->init(argc, argv); 

  orca::PointCloudData pc = pcci->getPointCloud(); 
  cout << "Got new pointcloud. Has a total of " << pc.points.size() << " xyz values " << endl;

  QMainWindow *mw = new QMainWindow();
  PointCloudViewer pcv(NULL); 
  pcv.setPointCloud(pc); 
  mw->setCentralWidget(&pcv); 
  mw->setWindowTitle("RescueGUI: Map only mode"); 
  mw->show(); 
  a.connect( &a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()) );
  return a.exec();
  return 0;	
}

