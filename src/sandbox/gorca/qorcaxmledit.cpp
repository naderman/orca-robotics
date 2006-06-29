/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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
