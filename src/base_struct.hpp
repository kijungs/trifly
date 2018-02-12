#ifndef _BASE_STRUCT_HPP
#define _BASE_STRUCT_HPP

#include "macro.hpp"
#include "option.hpp"

#include <fstream>
#include <cstring>
#include <limits>

using namespace std;

// if VID or MID is changed, change types in MPIIO() accordingly
typedef unsigned int VID; // Vertex ID
typedef short MID;

const VID INVALID_VID(numeric_limits<VID>::max());
const MID INVALID_MID(numeric_limits<MID>::max());

struct Edge 
{
	static const unsigned short szAttr 	= 2;

	VID	src;
	VID dst;

	Edge(): src(INVALID_VID), dst(INVALID_VID) {}
	Edge(VID iSrc, VID iDst): src(iSrc), dst(iDst) {}

	//Edge(const Edge &iEdge): src(iEdge.src), dst(iEdge.dst) {}
	inline bool operator==(const Edge& iEdge) const 
	{
		return (src == iEdge.src) && (dst == iEdge.dst);
	}
	inline bool operator!=(const Edge& iEdge) const 
	{	
		return !operator==(iEdge);
	}

};

struct ElemCnt
{
	static const unsigned short szAttr = 2;
	VID		vid;
	double	cnt;
	inline void setValue(VID iVid, double iCnt)
	{
		vid = iVid;
		cnt = iCnt;
	}
};

inline ostream& operator<<(ostream &os, const Edge &e){
	os << "(" << e.src << ", " << e.dst << ")" << "\t";
	return os;
}

class EdgeParser
{
_PRIVATE:
	fstream fp;

public:
	EdgeParser(){} // prevent default constructor.
	EdgeParser(const char* filename): fp(filename, std::fstream::in) {}

	inline bool getEdge(Edge &oEdge)
	{
		//string temp; 
		if (fp.eof())
		{
			return false;
		}

		fp >> oEdge.src >> oEdge.dst;// >> temp;
		if (fp.eof())
		{
			return false;
		}

		return true;
	}

	inline void rewind()
	{
		fp.seekg(0, ios_base::beg);
	}
};


#endif // #ifndef _BASE_STRCUT_HPP
