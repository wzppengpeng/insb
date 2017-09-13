#ifndef _SEARCH_H_
#define _SEARCH_H_
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
#include "ifs.h"
#include <unistd.h>

using namespace std;

class CSearch
{
public:
	CSearch(){}
	~CSearch(){}

	int  loadRef(string refPath);
	int  build_ifs(string dbpath, string rankdir, int cluster_num);
	int  batch_search(string queryList);
	int  search_query(string queryfeapath);
	int  search_queryexpand(string queryfeapath);
	
	int  asymbuild_ifs(string dbpath, string rankdir);//compute asymmetrical dissimilarity
	int  asymsearch_query(string queryfeapath);	//compute asymmetrical dissimilarity

	int  saveRankList(string dstPath, vector<CScoreDes>& scores);
	int  saveRankList(string dstPath, vector<CScore>& scores);
	void calIdf();
	void calRootNorm();
	void calL2Norm();

	float calCosSim(vector<int>& queryIdx, vector<float>& queryVal, int refIdx);    //the larger, the more similar
	float calRootSim(vector<int>& queryIdx, vector<float>& queryVal, int refIdx);
	float calRootDist(vector<int>& queryIdx, vector<float>& queryVal, int refIdx);
	float calL2Dist(vector<int>& queryIdx, vector<float>& queryVal, int refIdx);
	float calL2DistWithoutNorm(vector<int>& queryIdx, vector<float>& queryVal, int refIdx);
	float calL1Dist(vector<int>& queryIdx, vector<float>& queryVal, int refIdx);
	float calInterSim(vector<int>& queryIdx, vector<float>& queryVal, int refIdx);
        float calAsymDissim(vector<int>& queryIdx, vector<float>& queryVal,int refIdx);   //converted to similarity. the larger, the more similar
	void  calAsymDissimw(vector<int>& queryIdx, vector<float>& queryVal);

	int Loadroiidx(string roivwfile);//Load visual word idx of query Roi region
	int Loadroivwweight(string roivwfile);//Load visual word idx & weight of query Roi region

	int search_queryexpan(string queryfeapath);
private:
	double m_disw;
	vector<vector<float> > iVwValVec;
	vector<vector<int> > iVwIdxVec;
	vector<string> iFileNameVec;
	CInvertFile m_invert_file;
//	vector<index_t> roivwidx;
	map<string,set<index_t> > roivwidx;
	map<string,map<index_t,float> > roivwordweight;
};

#endif
