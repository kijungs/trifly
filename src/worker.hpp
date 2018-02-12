#ifndef _WORKER_HPP_
#define _WORKER_HPP_

#include "source.hpp"
#include <unordered_map>
#include <set>
#include <stdlib.h>     /* srand, rand */
#include <random>
#include <vector>
#include "time.h"

class Worker {
_PRIVATE:

	const int k; // maximum number of edges that can be stored
	std::vector<Edge> samples; // sampled edges
	long n; // maximum number of edges sent to so far with doStore=1
	double globalCnt; // global triangle count
	std::unordered_map<VID, float> nodeToCnt; // local triangle count
	std::unordered_map<VID, std::set<VID>> nodeToNeighbors; // sampled graph


	std::default_random_engine generator; // random real number generator
	std::uniform_real_distribution<double> distribution;


public:

	Worker(int k, unsigned int seed);
	//Worker(const Worker &iWorker): k(iWorker.k), n(iWorker.n), globalCnt(iWorker.globalCnt) {}
	//Worker& operator=(const Worker& t){
	//	n = t.n;
	//	globalCnt = t.globalCnt;
	//}

	void updateCnt(const Edge &iEdge);

	int deleteEdge();

	// Independent part
	void processEdge(const Edge &iEdge);
	double getGlobalCnt();
	std::unordered_map<VID, float> & getLocalCnt();
};



#endif // #ifndef _WORKER_HPP_
