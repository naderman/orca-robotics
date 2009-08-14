#ifndef HYDROPATHPLANSPARSESKEL_SOLVE_H
#define HYDROPATHPLANSPARSESKEL_SOLVE_H

#include <hydropathplan/sparseskel/sparseskel.h>

namespace hydropathplan {
namespace sparseskel {

// Non-member functions
void findShortestPath( const ContiguousSparseSkel &cSkel,
                       const SparseSkelNode *start,
                       const SparseSkelNode *goal,
                       std::vector<const SparseSkelNode *> &path );


}
}

#endif
