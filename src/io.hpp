#ifndef _IO_HPP_
#define _IO_HPP_

#include "base_struct.hpp"
#include <mpi.h>
#include <cstdlib>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <stddef.h>
#include <ctime>
#include <sys/time.h>

class MPIIO {
_PRIVATE:
/*
 *		Sub-class and static member and methods
 */
	class EdgeContainer {
	_PRIVATE:
		MID							mid;
		unsigned int				bit;
		unsigned int				qit;
		bool						isEmpty;
		double						ioCPUTime; // cpu time used for MPI communication

	public:
		Edge						*buf[2];
		MPI_Request 				req[2];

		EdgeContainer(): bit(0), qit(0), isEmpty(true), ioCPUTime(0),
				buf{nullptr, nullptr}, req{MPI_REQUEST_NULL, MPI_REQUEST_NULL}{}

		~EdgeContainer()
		{
			if (buf[0] != nullptr)
			{
				delete[] buf[0];
				buf[0] = nullptr;
			}

			if (buf[1] != nullptr)
			{
				delete[] buf[1];
				buf[1] = nullptr;
			}

			waitIOCompletion(req[0]);
			waitIOCompletion(req[1]);
		}

		inline bool init(MID iMid)
		{
			mid = iMid;
			buf[0] = new Edge[lenBuf];
			buf[1] = new Edge[lenBuf];
			return true;
		}

		// Methods for receiver
		inline void getNext(Edge &oEdge)
		{
			if (isEmpty)
			{
				clock_t begin = clock();

				MPIIO::IrecvEdge(buf[qit], req[qit]);
				waitIOCompletion(req[qit]);
				MPIIO::IrecvEdge(buf[(qit+1)%2], req[(qit+1)%2]);

				ioCPUTime += double(clock() - begin);

				isEmpty = false;
			}
			else if (bit == lenBuf)
			{
				clock_t begin = clock();

				bit = 0;
				MPIIO::IrecvEdge(buf[qit], req[qit]);
				qit = (qit + 1) % 2;
				waitIOCompletion(req[qit]);

				ioCPUTime += double(clock() - begin);
			}

			oEdge = buf[qit][bit++];

			return;
		}

		void cleanup(){
			int flag(0);
			MPI_Status st;
			for (int i = 0; i < 2; i++)
			{
				if (req[i] != MPI_REQUEST_NULL)
				{
					MPI_Test(&req[i], &flag, &st);
					if (flag == false)
					{
						MPI_Cancel(&req[i]);
					}
					req[i] = MPI_REQUEST_NULL;
				}
			}
		}

		// Methods for sender
		inline bool putNext(const Edge &iEdge)
		{

			buf[qit][bit++] = iEdge;
			if (bit == lenBuf)
			{
				clock_t begin = clock();

				bit = 0;
				MPIIO::IsendEdge(buf[qit], mid, req[qit]);
				qit = (qit + 1) % 2;
				waitIOCompletion(req[qit]);

				ioCPUTime += double(clock() - begin);
			}

			return true;
		}

		void flushSend()
		{

			clock_t begin = clock();

			if (bit != 0)
			{
				MPIIO::IsendEdge(buf[qit], mid, req[qit]);
			}

			for (int i = 0; i < 2; i++)
			{
				waitIOCompletion(req[i]);
			}

			ioCPUTime += double(clock() - begin);
		}
	};

	static const int 		TAG_STREAM = 0;
	static const int 		TAG_RET	= 1;
	static MPI_Datatype 	MPI_TYPE_EDGE;
	static MPI_Datatype 	MPI_TYPE_ELEMCNT;
	static const Edge 		END_STREAM;
	static unsigned short	lenBuf;

	inline static void waitIOCompletion(MPI_Request &iReq)
	{
		MPI_Status 	st;
		(MPI_REQUEST_NULL != iReq) && MPI_Wait(&iReq, &st);
		iReq = MPI_REQUEST_NULL;
		return;
	}

	static bool IrecvEdge(Edge *buf, MPI_Request &iReq);
	static bool IsendEdge(Edge *buf, int dst, MPI_Request &iReq);


/*
 *		Sub-class and static member and methods
 */

	MPI_Request				req;
	int 					rank;
	int 					szProc;
	int 					workerNum; // number of actual workers
	vector<EdgeContainer>	eBuf;
	long						commCostDistribute;
    long						commCostGather;
	long						ioCPUTime;


public:

	MPIIO(int &argc, char** &argv);
	~MPIIO(){}

	// initialize
	void init(int buffersize, int workerNum);
	void cleanup();

	// whether this thread is a master or not
	bool isMaster();

	// whether this thread is a valid worker or not
	// bool isActiveWorker();

	// worker Id
	MID getWorkerId();

	// communication cost (logical)
	long getCommCostDistribute();
	long getCommCostGather();

	// Edge
	bool sendEdge(const Edge &iEdge, MID dst);
	bool bCastEdge(Edge& iEdge, MID dst1, MID dst2);
	bool recvEdge(Edge &oEdge);

	// Receiving data
	bool sendCnt(double gCnt, unordered_map<VID, float> &lCnt);
    bool recvCnt(VID maxVId, double &gCnt, std::vector<float> &lCnt);

	// CPU Time used for MPI communication
	double getIOCPUTime();

	// send computational time info
	bool sendTime(double compTime);
	bool recvTime(double &compTimeMax, double &compTimeSum);

	// Control flow
	bool sendEndSignal();

	// Get variable
	int getRank(){ return rank;}
	int getSzProc(){ return szProc;}

};

#endif // _IO_HPP_
