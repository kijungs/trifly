#ifndef _SOURCE_HPP_
#define _SOURCE_HPP_

#include "base_struct.hpp"
#include "io.hpp"

#include <mpi.h>
#include <set>
#include <unordered_map>
#include <vector>
#include <iostream>

class Source {
_PRIVATE:

    // max node id
    VID maxVId;

public:

    Source();

	// Only for MPI
    bool processEdge(Edge &iEdge, MID &oDstMID1, MID &oDstMID2);

    VID getMaxVId();
};

#endif // _SOURCE_HPP_
