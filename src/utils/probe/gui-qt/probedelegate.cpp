/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2006-2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>

#include <orcaqcm/orcaqcm.h>  // for roles

#include "probedelegate.h"

using namespace std;
using namespace probe;

ProbeDelegate::ProbeDelegate(QObject *parent)
        : QItemDelegate(parent),
          isOn_(true) 
{
}

void ProbeDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const
{
    if ( !isOn_ ) {
//         cout<<"painting invisible in delegate"<<endl;
        return;
    }

    // if disconnected, paint as disabled
    if ( !index.model()->data(index, orcaqcm::OcmModel::ConnectionRole).toInt() ) {
    
        //cout<<"ProbeDelegate::paint: i'm am disabled"<<endl;
                
        QStyleOptionViewItem option2(option);
        // set the enabled bit to 0
        option2.state &= ~QStyle::State_Enabled;
        
        QItemDelegate::paint( painter, option2, index );
    }
    else {
        QItemDelegate::paint( painter, option, index );
    }
}
