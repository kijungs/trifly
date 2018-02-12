#include "source.hpp"

Source::Source(): maxVId(128)
{

}

VID Source::getMaxVId()
{
    return maxVId;
}

/**
 *
 * @param iEdge input edge
 * @param oDstMID1 destination machine 1
 * @param oDstMID2 destination machine 2
 * @return whether to broadcast
 */
bool Source::processEdge(Edge &iEdge, MID &oDstMID1, MID &oDstMID2)
{

    VID src = iEdge.src;
    VID dst = iEdge.dst;
    if (src > maxVId) {
        maxVId = src;
    }
    if (dst > maxVId) {
        maxVId = dst;
    }
	return true;
}

