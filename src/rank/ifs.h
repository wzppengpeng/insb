#ifndef _INVERTED_FILE_SYSTEM_H_
#define _INVERTED_FILE_SYSTEM_H_


#include "ffs.h"
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>
#include <algorithm>
#include "common.h"
#include <set>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

using namespace std;

typedef int index_t;


struct poolword_t
{
	index_t imgidx;
	float   value;
};


//typedef map<index_t, poolword_t>::iterator pw_map_iter;


class CInvertFile
{
public:

	CInvertFile(){};
	~CInvertFile();

	int build_ifs(int dictsize, string dblist);
	void set_rank_path(string rankdir);

	int queryexpand_ffs(map<index_t, float> &query, string queryName,map<index_t,float>& roivwordweight);//query expansion
	vector<CScoreDesidx> search_ifs(map<index_t, float>& query, string queryName);//return most similar image idx

	int asymbuild_ifs(int dictsize, string dblist);
	int asymsearch_ifs(map<index_t, float> &query, string queryName);

	int monitor();
	int loadVheadlist(string filename);

private:
	int init_ifs();
	int load_base(string dbpath);
    int asymload_base(string dbpath);
	int cal_rootsim(map<index_t, float> &query);
	int cal_histintsim(map<index_t, float> &query);
	int rank_and_save(string queryName,vector<CScoreDesidx>& filelist);
	int asymrank_and_save(string queryName);

	int cal_IDFrootsim(map<index_t, float> &query);
	void calIdf();
	void calE_Idf(double arfa);
	void Idfweightquery(map<index_t, float> &query);

	float cal_selfIDFrootsim(map<index_t, float>& vwweightquery,map<index_t, float> &query);//compute similarity with self
	int cal_asymdissimi_con(map<index_t, float> &query);//asymmetrical dissimilarity constant weight value
	void calAsymDissimw(map<index_t, float> &query);	//compute weight value
	int calAsymDissim();					//the larger, the more dissimilar

	int save_rank_list(string dstPath, vector<CScoreDesidx>& scores,vector<CScoreDesidx>& filelist);
	int asymsave_rank_list(string dstPath, vector<CScoreDes>& scores);
	int m_dict_size;
	string m_img_path;                  	   //path that stores keypoints
	string m_rank_path;                        //path that stores ranked list
	vector<string> m_imglist;           	   //image name list
	vector<int> m_totalpoint;
	set<string> headlist;			//pic name string in the Video head
	//vector<vector<descp_t> > m_ifs;      	   //inverted file that keeps SIFT
	//vector<vector<int> > m_angle_diff_fwd;    //WGC hist that keeps angle diff
	//vector<vector<int> > m_scale_diff_fwd;    //WGC hist that keeps scale diff
	//vector<vector<int> > m_angle_diff_rvs;    //WGC hist that keeps angle diff
	//vector<vector<int> > m_scale_diff_rvs;    //WGC hist that keeps scale diff
	
	vector<vector<poolword_t> > m_ifs;      	   //inverted file that keeps projected features
	vector<double> m_similarity;			//the larger, the more similar
	double m_disw;	//weight value of Asymmetrical metric
	vector<float> m_idf;
	float m_selfsimi;
	vector<double> m_T;
	double m_T_num;	//Asymmetrical dissimilarity weight value->numerator
	CFfs m_ffs;
};

#endif
