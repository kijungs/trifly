#ifndef TRIANGLE_DIST_RUN_HPP
#define TRIANGLE_DIST_RUN_HPP

#include "base_struct.hpp"
#include "source.hpp"
#include "worker.hpp"
#include <vector>
#include <iostream>
#include <unordered_map>
#include <sys/time.h>
#include <fstream>
#include <sstream>
#include <iomanip>

#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <limits>

// run actual mpi implementation
double run_mpi(const char* filename, MPIIO &hIO, int workerNum, int memSize, int lenBuf, double tolerance, unsigned int seed, std::vector<float> & oLocalCnt, double &srcCompCost, double &workerCompCostMax, double &workerCompCostSum);

// run experiment with mpi
void run_exp (const char* input, const char* outPath, MPIIO &hIO, int workerNum, int memSize, int repeat, int bufLen=1000, double tolerance=0.2);

void print_cnt(const char* outPath, double globalCnt, const std::vector<float> &localCnt, int id = 0);

#endif //TRIANGLE_DIST_RUN_HPP
