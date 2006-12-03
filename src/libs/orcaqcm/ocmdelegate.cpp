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

#include "ocmdelegate.h"
#include "ocmmodel.h"   // for roles

using namespace std;

namespace qorcacm
{

void OcmDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const
{
    // if disconnected, paint as disabled
    if ( !index.model()->data(index, OcmModel::ConnectionRole).toInt() ) {
    
        //cout<<"OcmDelegate::paint: i'm am disabled"<<endl;
                
        QStyleOptionViewItem option2(option);
        // set the enabled bit to 0
        option2.state &= ~QStyle::State_Enabled;
        //option2.palette.setCurrentColorGroup ( QPalette::Disabled );
        
        QItemDelegate::paint( painter, option2, index );
    }
    else {
        QItemDelegate::paint( painter, option, index );
    }
}

} // namespace
