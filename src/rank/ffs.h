#ifndef _FFS_H_
#define _FFS_H_
#include <string>
#include <vector>
#include <set>
#include <map>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <cmath>

#include <cstdio>
#include <ctime>
#include <cstdlib>

#include "common.h"

using namespace std;

typedef int index_t;

class CFfs
{
public:
	CFfs();
	~CFfs();
	int load_ffs(string dbpath);
	map<index_t,float> get_newquery();
	int find_dbase(index_t imgid);//return the imgid-image feature
	int avg_query_expand(map<index_t,float>& query,vector<index_t>& imgidvec);//average query expansion

	int avg_queryratio_expand(map<index_t,float>& query,vector<CScoreDesidx>& imgidvec,float selfsimi=1000000);//average query expansion according to similarity
	int avg_expandrenorm_ratio(map<index_t,float>& vwordweight,map<index_t,float>& query,vector<CScoreDesidx>& imgidvec);//first L1renorm topresult;then,average queryexpand
	
	int vwrenorm_ffs(map<index_t,float>& ffsitem,map<index_t,float>& vwordweight);//L1renorm
	int loadone_item(index_t* idxlist,float* valuelist,int featuredim,int fileidx);//Load one image of FFS
	//int weight_vword(map<index_t,float>& vecdata,map<index_t,float>& vwordweight);//return ROI-word-weighted query

private:
	vector<map<index_t,float> > m_ffstable;//ffile system 
	map<index_t,float> m_newquery;//expanded query
	
};

#endif
