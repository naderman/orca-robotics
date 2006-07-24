/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>

#include <qfile.h>
#include <qmessagebox.h>

#include "qorcarepdom.h"


QorcaRepositoryDom::QorcaRepositoryDom( const QString fileName )
{
    // read the XML file and create DOM tree
    QFile opmlFile( fileName );
    if ( !opmlFile.open( QIODevice::ReadOnly ) )
    {
        QMessageBox::critical( 0,"Critical Error","Cannot open file" );
        return;
    }

    QString errorMsg;
    int errorLine;
    int errorColumn;
    if ( !this->setContent( &opmlFile, false, &errorMsg, &errorLine, &errorColumn ) )
    {
        std::cout<<errorMsg.toStdString()<<" line:"<<errorLine<<" col:"<<errorColumn<<std::endl;
        QMessageBox::critical( 0,"Critical Error","Parsing error for file");
        opmlFile.close();
        return;
    }
    opmlFile.close();

//     // get the header information from the DOM
//     QDomElement root = this->documentElement();
//     QDomNode node;
//     node = root.firstChild();
//     while ( !node.isNull() )
//     {
//         if ( node.isElement() && node.nodeName() == "head" )
//         {
//             QDomElement header = node.toElement();
//             parseHeader( header );
//             break;
//         }
//         node = node.nextSibling();
//     }
//     // create the tree view out of the DOM
//     node = root.firstChild();
//     while ( !node.isNull() )
//     {
//         if ( node.isElement() && node.nodeName() == "body" )
//         {
//             std::cout<<"found body"<<std::endl;
//             QDomElement body = node.toElement();
//             parseRepository( body );
//             break;
//         }
//         node = node.nextSibling();
//     }
}

QorcaRepositoryDom::~QorcaRepositoryDom()
{}

void QorcaRepositoryDom::parseHeader( const QDomElement &header )
{
    // visit all children of the header element and look if you can make
    // something with it
    QDomNode node = header.firstChild();
    while ( !node.isNull() )
    {
        if ( node.isElement() )
        {
            // case for the different header entries
            if ( node.nodeName() == "title" )
            {
                QDomText textChild = node.firstChild().toText();
                if ( !textChild.isNull() )
                {
//                    setColumnText( 0, textChild.nodeValue() );
                }
            }
        }
        node = node.nextSibling();
    }
}

void QorcaRepositoryDom::parseRepository( const QDomElement &body )
{
    //QListViewItem *compItem = 0;
    // parent here is body
    QDomNode node = body.firstChild();
    while ( !node.isNull() )
    {
        if ( node.isElement() && node.nodeName() == "component" )
        {
            // add a new list view item for the outline
            //compItem = new QListViewItem( this, compItem );
            //compItem->setText( 0, "Orca Component" );
            // parse component info
            parseComponent( node.toElement() );
        }
        node = node.nextSibling();
    }
}

void QorcaRepositoryDom::parseComponent( const QDomElement &component )
{
    //QListViewItem *portItem = 0;

    // look for name, ports, etc.
    QDomNode node = component.firstChild();
    while ( !node.isNull() )
    {
        if ( node.isElement() && node.nodeName() == "name" )
        {
            //compItem->setText( 0, node.toElement().attribute( "text" ) );
        }
        else if ( node.isElement() && node.nodeName() == "port" )
        {
            //portItem = new QListViewItem( compItem, portItem );
            //portItem->setText( 0, "Anonymous Ports" );
            parsePort( node.toElement() );
        }
        node = node.nextSibling();
    }
}

void QorcaRepositoryDom::parsePort( const QDomElement &portElement )
{
//    portItem->setText( 0, portElement.attribute( "name" ) );

    // look for interfaces
    //     QDomNode node = parentElement.firstChild();
    //     while ( !node.isNull() )
    //     {
    //         if ( node.isElement() && node.nodeName() == "port" )
    //         {
    //             // add a new list view item for the outline
    //             thisItem = new QListViewItem( parentItem, thisItem );
    //             thisItem->setText( 0, node.toElement().attribute( "type" ) );
    //             // recursive build of the tree
    //             //parsePort( thisItem, node.toElement() );
    //         }
    //         node = node.nextSibling();
    //     }
}


