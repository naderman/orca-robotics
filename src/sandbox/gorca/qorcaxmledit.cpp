/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>

#include "qorcaxmledit.h"

QorcaXmlEdit::QorcaXmlEdit( QDomElement& element, QWidget *parent, const char *name, Qt::WFlags f )
        : Q3ListView( parent, name, f ), xml_(&element) {
    setSorting( 0, true );
    addColumn( "Element" );
    addColumn( "Value" );
    setRootIsDecorated( true );

    xmlToTree();
}

QorcaXmlEdit::~QorcaXmlEdit() {
    treeToXml();
}

void QorcaXmlEdit::xmlToTree() {
    Q3ListViewItem *topItem = new Q3ListViewItem( this );
    topItem->setText(0, xml_->tagName() );
    topItem->setOpen( true );

    QDomNamedNodeMap attrmap = xml_->attributes();
    QDomNode node;
    for ( unsigned int i=0; i<attrmap.length(); i++ ) {
        Q3ListViewItem *item = new Q3ListViewItem( topItem );
        item->setRenameEnabled( 1, true );
        node = attrmap.item(i);
        item->setText(0, node.toAttr().name() );
        item->setText(1, node.toAttr().value() );
    }
}


void QorcaXmlEdit::treeToXml() {
    Q3ListViewItem *topItem = this->firstChild();

    Q3ListViewItemIterator it( topItem );
    // get rid of the root item itself
    ++it;
    while ( it.current() ) {
        xml_->setAttribute( it.current()->text(0), it.current()->text(1) );
        std::cout<<it.current()->text(0).toStdString()<<" "<<it.current()->text(1).toStdString()<<std::endl;
        ++it;
    }
}
