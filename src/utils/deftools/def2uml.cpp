/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#include <orcadef/orcadef.h>
#include <orcaquml/orcaquml.h>

#include <QtGui>

#include <iostream>
using namespace std;

void 
usage()
{
    cout << "USAGE"<<endl;
    cout << "def2uml file [OPTIONS]"<<endl;
    cout << "  file\t .def component definition file."<<endl;
    cout << "OPTIONS"<<endl;
    cout << "  -h, --help\n\tPrints this."<<endl;
    cout << "  -d, --display\n\tDisplays the UML symbol in a window, in addition to saving an image file."<<endl;
    cout << "  -V, --verbose\n\tPrints extra debugging information."<<endl;
    cout << "EXAMPLES"<<endl;
    cout << "  generateuml /opt/orca2/def/localnav.def"<<endl;
    cout << "  for file in /opt/orca2/def/*; do echo $file; def2uml $file; done"<<endl;
}

 int main(int argc, char **argv)
{
    std::string defName = "";
    bool gotDefName=false;
    bool isVerbose=false;
    bool isDisplay=false;

    for ( int i=1; i < argc; i++ )
    {
        if ( !strcmp(argv[i],"--verbose") || !strcmp(argv[i],"-V") )
        {
            isVerbose=true;
        }
        else if ( !strcmp(argv[i],"--display") || !strcmp(argv[i],"-d") )
        {
            isDisplay=true;
        }
        else if ( !strcmp(argv[i],"--help") || !strcmp(argv[i],"-h") )
        {
            usage();
            exit(0);
        }
        else if ( !gotDefName )
        {
            defName=argv[i];
            gotDefName=true;
        }
        else
        {
            usage();
            exit(1);
        }
    }
    if ( !(gotDefName ) )
    {
        usage();
        exit(1);
    }
    std::string imgName = "output.png";

    orcadef::ComponentDef def;
    try {
        orcadef::parseDefFile( defName, def );
    }
    catch ( std::string &e )
    {
        cout<<": Error parsing '" << defName << "': " << e << endl;
        exit(1);
    }
    
    if ( isVerbose ) {
        cout<<orcadef::toString(def)<<endl;
    }

    QApplication app(argc, argv);

    orcaquml::ComponentModel model( def );
    QGraphicsScene scene;

    QGraphicsItem *item = new orcaquml::Component( model, &scene );
    item->setPos(QPointF(5.0, 5.0));

    QRectF field = item->boundingRect() | item->childrenBoundingRect();
    scene.setSceneRect( field.adjusted( 0.0,0.0, 15.0,15.0 ) );

    // save
    QPixmap pixmap( scene.sceneRect().size().toSize() );
    pixmap.fill();
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    scene.render(&painter); 
    pixmap.save( QString::fromStdString( model.def.tag ).toLower() + ".png" );

    // display
    if ( isDisplay )
    {
        QGraphicsView view;
        view.setRenderHint(QPainter::Antialiasing, true);
        view.setDragMode(QGraphicsView::RubberBandDrag);
        view.setScene( &scene );
        view.setWindowTitle("Orca: UML symbol generator");
        view.show();
    
        return app.exec();
    }
}
