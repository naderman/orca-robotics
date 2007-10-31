/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef QORCACM_MODEL_HANDLER_H
#define QORCACM_MODEL_HANDLER_H

#include <QObject>
#include <hydroutil/notify.h>

#include <orcacm/types.h>
#include <orcaqcm/networkhandler.h>
#include <orcaqcm/ocmmodel.h>

class QApplication;

namespace orcaqcm
{

class ModelHandler : public QObject, public hydroutil::NotifyHandler<orcacm::ComponentData>
{
    Q_OBJECT
    
public:
    ModelHandler( orcaqcm::NetworkHandler & networkHandler );
    virtual ~ModelHandler();

    QAbstractItemModel* model() { return &model_; };

    void clearModel() { model_.clear(); };
    
    virtual void handleData( const orcacm::ComponentData & obj );
    
    // Called by the Qt runtime.
    virtual void customEvent( QEvent* e );

private:
    orcaqcm::OcmModel model_;
};

} // namespace

#endif
