#include "rxdata.h"
#include <iostream>

using namespace std;

namespace segwayrmp {

RxData::RxData( RmpModel rmpModel,
                const orcaice::Context &context )
    : rawData_(context),
      converter_(rmpModel)
{
}

}

