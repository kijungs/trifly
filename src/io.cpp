#include "io.hpp"

MPI_Datatype 		MPIIO::MPI_TYPE_EDGE;
MPI_Datatype 		MPIIO::MPI_TYPE_ELEMCNT;
const Edge 			MPIIO::END_STREAM(INVALID_VID, INVALID_VID);

unsigned short		MPIIO::lenBuf;

MPIIO::MPIIO(int &argc, char** &argv)//, bit(0), lenCQ(1), cqit(0)
{
	// Establish connection
	MPI_Init(&argc, &argv);

	// Get connection information
	MPI_Comm_size(MPI_COMM_WORLD, &szProc);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	// Initialize and Register struct EDGE, ELEMCNT information
	int          lenAttr[Edge::szAttr] = {1, 1};
	MPI_Datatype arrType[Edge::szAttr] = {MPI_UNSIGNED, MPI_UNSIGNED};

	MPI_Aint     offsets[Edge::szAttr];
	offsets[0] = offsetof(Edge, src);
	offsets[1] = offsetof(Edge, dst);
	MPI_Type_create_struct(Edge::szAttr, lenAttr, offsets, arrType, &MPI_TYPE_EDGE);
	MPI_Type_commit(&MPI_TYPE_EDGE);

	arrType[0] = MPI_UNSIGNED;
	arrType[1] = MPI_DOUBLE;
	offsets[0] = offsetof(ElemCnt, vid);
	offsets[1] = offsetof(ElemCnt, cnt);
	MPI_Type_create_struct(ElemCnt::szAttr, lenAttr, offsets, arrType, &MPI_TYPE_ELEMCNT);
	MPI_Type_commit(&MPI_TYPE_ELEMCNT);
}

// Initialize requests and buffers
void MPIIO::init(int lenBuf, int workerNum)
{
	commCostDistribute = 0;
    commCostGather = 0;
    ioCPUTime = 0;

    eBuf.clear();
    MPIIO::lenBuf = lenBuf;
    MPIIO::workerNum = workerNum;

    if (rank == MPI_MASTER)
    {
        eBuf.resize(workerNum);
        for (int i = 0; i < workerNum; i++)
        {
            eBuf[i].init(i);
        }
    }
    else
    {
        eBuf.resize(1);
        eBuf[0].init(getWorkerId());
    }
}


bool MPIIO::isMaster()
{
    return rank == MPI_MASTER;
}

//bool MPIIO::isActiveWorker()
//{
//    return rank <= workerNum;
//}

MID MPIIO::getWorkerId()
{
    return (MID)(rank - 1);
}

long MPIIO::getCommCostDistribute()
{
    return commCostDistribute;
}

long MPIIO::getCommCostGather()
{
    return commCostGather;

}

void MPIIO::cleanup()
{
	MPI_Finalize();
}

bool MPIIO::bCastEdge(Edge &iEdge, MID dst1, MID dst2)
{
	Edge tmpEdge(iEdge);
	for (int mit = 0; mit < workerNum; mit++)
	{
		eBuf[mit].putNext(tmpEdge);
	}
	commCostDistribute += workerNum;

	return true;
}

bool MPIIO::IrecvEdge(Edge *buf, MPI_Request &iReq)
{
	return (MPI_SUCCESS == MPI_Irecv(buf, lenBuf, MPI_TYPE_EDGE, MPI_MASTER, TAG_STREAM, MPI_COMM_WORLD, &iReq));
	//waitIOCompletion(iReq);
}

bool MPIIO::IsendEdge(Edge *buf, int mid, MPI_Request &iReq){
	MPI_Isend(buf, lenBuf, MPI_TYPE_EDGE, mid + 1, TAG_STREAM, MPI_COMM_WORLD, &iReq);
	return true;
}

bool MPIIO::recvEdge(Edge &oEdge)
{
	eBuf[0].getNext(oEdge);
	if (oEdge == END_STREAM)
	{
		eBuf[0].cleanup();
	}
	return (oEdge != END_STREAM);
}

bool MPIIO::sendEndSignal()
{
	Edge signal(END_STREAM);
	for (int mit = 0; mit < workerNum; mit++)
	{
		eBuf[mit].putNext(signal);
		eBuf[mit].flushSend();
	}
	return true;
}

// 
bool MPIIO::sendCnt(double gCnt, unordered_map<VID, float> &lCnt)
{
    clock_t begin = clock();
	MPI_Reduce(&gCnt, nullptr, 1, MPI_DOUBLE, MPI_SUM, MPI_MASTER, MPI_COMM_WORLD);
    VID maxVId;
    MPI_Bcast(&maxVId, 1, MPI_UNSIGNED, MPI_MASTER, MPI_COMM_WORLD);
    ioCPUTime += double(clock() - begin);


    float* lCntArr = new float[maxVId+1];
    std::fill_n(lCntArr, maxVId+1, 0.0);
    unordered_map<VID, float>::const_iterator it;
    for (it = lCnt.begin(); it != lCnt.end(); it++) {
        lCntArr[it->first] = it->second;
    }

    begin = clock();
    MPI_Reduce(lCntArr, nullptr, maxVId+1, MPI_FLOAT, MPI_SUM, MPI_MASTER, MPI_COMM_WORLD);
    ioCPUTime += double(clock() - begin);

    delete lCntArr;
    return true;

    /*
    if(szLCnt > 0) {

        ElemCnt *cntMap = new ElemCnt[szLCnt];

        unordered_map<VID, double>::const_iterator it;
        int idx(0);
        for (it = lCnt.begin(); it != lCnt.end(); it++) {
            cntMap[idx].vid = it->first;
            cntMap[idx++].cnt = it->second;
        }

        //cout << "Machine #" << rank << " sent " << idx << " key-value pairs " << endl;

        MPI_Request req(MPI_REQUEST_NULL);
        MPI_Status st;
        MPI_Isend(cntMap, szLCnt, MPI_TYPE_ELEMCNT, MPI_MASTER, TAG_RET, MPI_COMM_WORLD, &req);
        MPI_Wait(&req, &st);

        delete[] cntMap;
    }*/
}

bool MPIIO::recvCnt(VID maxVId, double &gCnt, std::vector<float> &lCnt)
{
	//TODO. unsinged int -> unsigned long long?

	double empty = 0;
    clock_t begin = clock();
	MPI_Reduce(&empty, &gCnt, 1, MPI_DOUBLE, MPI_SUM, MPI_MASTER, MPI_COMM_WORLD);
    MPI_Bcast(&maxVId, 1, MPI_UNSIGNED, MPI_MASTER, MPI_COMM_WORLD);
    ioCPUTime += double(clock() - begin);

    float* lCntArr = new float[maxVId+1];
    std::fill_n(lCntArr, maxVId+1, 0.0);

    begin = clock();
    MPI_Reduce(MPI_IN_PLACE, lCntArr, maxVId+1, MPI_FLOAT, MPI_SUM, MPI_MASTER, MPI_COMM_WORLD);
    ioCPUTime += double(clock() - begin);

    commCostGather = (maxVId + 1) * (getSzProc()-1);

    lCnt.insert(lCnt.end(), &lCntArr[0], &lCntArr[maxVId]);
    delete lCntArr;

    /*
	unsigned int cnt = 0;
	unsigned int *szLCnt = new unsigned int[szProc];
	memset(szLCnt, 0, sizeof(unsigned int)*szProc);


	MPI_Gather(&cnt, 1, MPI_UNSIGNED, szLCnt, 1, MPI_UNSIGNED, MPI_MASTER, MPI_COMM_WORLD);

	szLCnt[0] = 0;
	// cout << "Count vector: [";
	for (int mit = 0; mit < szProc - 1; mit++)
	{
		// cout << szLCnt[mit+1] << ", ";
		szLCnt[0] = max(szLCnt[0], szLCnt[mit+1]);
        commCostGather += szLCnt[mit+1];
	}
	// cout << "]" <<  endl << "Max length: " << szLCnt[0] << endl;

	ElemCnt *cntBuf = new ElemCnt[szLCnt[0]];
	MPI_Status st;
	MPI_Request	req(MPI_REQUEST_NULL);

	VID tmpVid;
	for (int rit = 1; rit < szProc; rit++) // iterate over ranks [1..szProc - 1]
	{

        struct timeval diff, startTV, endTV; //debug
        gettimeofday(&startTV, NULL);

        if(szLCnt[rit] > 0) {

            MPI_Irecv(cntBuf, szLCnt[rit], MPI_TYPE_ELEMCNT, rit, TAG_RET, MPI_COMM_WORLD, &req);
            MPI_Wait(&req, &st);

            // cout << "From machine #" << rit << " receives " << szLCnt[rit] << endl;

            for (int eit = 0; eit < szLCnt[rit]; eit++) // iterate over machine id [0 .. szProc - 1)
            {
                tmpVid = cntBuf[eit].vid;
                lCnt[tmpVid] = lCnt[tmpVid] + cntBuf[eit].cnt;
            }
        }

        gettimeofday(&endTV, NULL);

        timersub(&endTV, &startTV, &diff); //debug

        double elapsedTime = diff.tv_sec * 1000 + diff.tv_usec / 1000 ;

        std::cout << rit << "\t" << elapsedTime << endl;


        //cout << endl;
	}

	delete[] szLCnt;
	delete[] cntBuf; */

	return true;
}

double MPIIO::getIOCPUTime()
{
	if (rank == MPI_MASTER)
	{
		double totalIOCPUTime = ioCPUTime;
		for (int i = 0; i < workerNum; i++)
		{
			totalIOCPUTime += eBuf[i].ioCPUTime;
		}
		return totalIOCPUTime;
	}
	else
	{
		return eBuf[0].ioCPUTime;
	}
}

bool MPIIO::sendTime(double compTime)
{
    MPI_Reduce(&compTime, nullptr, 1, MPI_DOUBLE, MPI_MAX, MPI_MASTER, MPI_COMM_WORLD);
    MPI_Reduce(&compTime, nullptr, 1, MPI_DOUBLE, MPI_SUM, MPI_MASTER, MPI_COMM_WORLD);
    return true;
}

bool MPIIO::recvTime(double &compTimeMax, double &compTimeSum)
{
	double empty = 0;
	MPI_Reduce(&empty, &compTimeMax, 1, MPI_DOUBLE, MPI_MAX, MPI_MASTER, MPI_COMM_WORLD);
    MPI_Reduce(&empty, &compTimeSum, 1, MPI_DOUBLE, MPI_SUM, MPI_MASTER, MPI_COMM_WORLD);
	return true;

}
