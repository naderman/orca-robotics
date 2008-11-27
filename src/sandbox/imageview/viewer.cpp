#include "viewer.h"
#include "qtviewer.h"
#include "opencvviewer.h"

using namespace imageview;

Viewer::Viewer( const orcaice::Context& context ) :
    context_(context)
{
}

//! Add new viewer types here.
Viewer* Viewer::factory( const std::string& type, const orcaice::Context& context )
{
  if(type == "QtViewer")     return new QtViewer( context );
  if(type == "OpenCVViewer") return new OpenCVViewer( context );
  
  throw BadViewerCreation(type);
}
 


