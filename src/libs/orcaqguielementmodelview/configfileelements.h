#ifndef ORCQQGEMV_CONFIGFILEELEMENTS_H
#define ORCQQGEMV_CONFIGFILEELEMENTS_H

#include <orcaqguielementmodelview/guielementmodel.h>
#include <orcaice/context.h>

namespace orcaqgemv {

    void
    loadElementsFromConfigFile( GuiElementModel &guiElementModel,
                                const orcaice::Context &context );

}

#endif
