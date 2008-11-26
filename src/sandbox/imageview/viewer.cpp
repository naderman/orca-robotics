#include "viewer.h"
#include "qtviewer.h"
#include "opencvviewer.h"

using namespace imageview;

//! Add new viewer types here.
Viewer* Viewer::factory(const std::string& type) throw(Viewer::BadViewerCreation) 
{
  if(type == "QtViewer")     return new QtViewer;
  if(type == "OpenCVViewer") return new OpenCVViewer;
  throw BadViewerCreation(type);
}
 


