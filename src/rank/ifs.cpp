#include "ifs.h"
#include "coutcolor.h"
#include <limits>
#include <float.h>

#include "files/files.hpp"
#include "files/path.hpp"
#include "log/log.hpp"

// #define REMOVEVIHEAD
//#define ROOT			//normal rootsimi
//#define HISTINTERSEC		//histgram intersection simi
//#define SIMILARITYNORM

#define CALIDFROOT		//IDF root simi
//#define STATISTICIDF 		//whether to calculate idf or E-idf
#define STATISTICEIDF		//whether to use E-idf
#define TOPRESULT 10 		//here useless
//#define FFS
//#define FFSDATASQRTNORM	//whether to renorm database vec ;for sift,useless
double eidf_arfa=20000;	//parameter of e-idf
CInvertFile::~CInvertFile()
{
	m_ifs.clear();
	m_imglist.clear();
	m_totalpoint.clear();
	m_idf.clear();
	m_T.clear();
	headlist.clear();
	m_ifs.clear();
}

void CInvertFile::set_rank_path(string rankdir)
{
	m_rank_path = rankdir + "/";
	string command = "mkdir -p ";
	command = command + m_rank_path;
	int res = system(command.c_str());
#ifdef FFS
	string tmppath=m_rank_path+"rerank/";
	command = "mkdir -p ";
        command = command + tmppath;
	system(command.c_str());
#endif
}

int CInvertFile::build_ifs(int dictsize, string dblist)
{
	struct timeval tpstart,tpend;
	float timeuse;
	gettimeofday(&tpstart,NULL);

	cout << YELLOW << "[notice] inverted file system: building ifs... " << RESET << endl; 

	m_dict_size = dictsize;
	if(m_dict_size <= 0)
	{
		cerr << RED << "[error] invalid dictionary size" << RESET << endl;
		return -1;
	}

	int ret;
	int i = 0;

	ret = init_ifs();

	vector<string> filenames;
    CHECK(wzp::file_system::get_all_files(dblist, filenames));
    for(size_t i = 0; i < filenames.size(); ++i) {
        ret = load_base(wzp::Path::join({dblist, filenames[i]}));
    }
#ifdef STATISTICEIDF
    calE_Idf(eidf_arfa);
#endif
#ifdef STATISTICIDF
    calIdf();//compute IDF weight value vector
#endif

    gettimeofday(&tpend,NULL);
    timeuse=1000000*(tpend.tv_sec-tpstart.tv_sec)+
        tpend.tv_usec-tpstart.tv_usec;
    timeuse/=1000000;
    cout << GREEN << "[notice]Used Time(s): " << timeuse << RESET << endl;

    cout << GREEN << "[success] build inverted file system success" << RESET << endl;
    cout<<"remained pic number of Video head:"<<headlist.size()<<endl;
	return 0;
}

int CInvertFile::monitor()
{
	ofstream oFile("ifs.size.txt",ios::out);
	int sum=0;
	int sizetmp=0;
	for(int j=0;j<m_dict_size;j++)
	{
		sizetmp=m_ifs[j].size();
		oFile << sizetmp << "/" << m_ifs[j].capacity() <<endl;
		sum+=sizetmp;
	}

	oFile<<sum<<endl;
	oFile.close();

}

int CInvertFile::init_ifs()
{
	m_similarity.clear();
	m_ifs.resize(m_dict_size);
#ifdef REMOVEVIHEAD	
	loadVheadlist("./tmp/VideoHeadlist");
	loadVheadlist("./tmp/BlackFrlist");

#endif
	m_idf.resize(m_dict_size);
	return 0;	
}

int CInvertFile::loadVheadlist(string filename)
{
	ifstream iFile(filename.c_str(),ios::in);
	if(!iFile)
	{
		cout<<"Load VideoHeadList ERROR!\n";
		return 0;
	}
	string sline;
	while(getline(iFile,sline))
	{
		istringstream ss(sline);
		string tmp;
		ss>>tmp;
		headlist.insert(tmp);
		sline.clear();
		ss.clear();
		ss.str("");
	}
	iFile.close();
	cout<<"Load Video Head list... total pic number:"<<headlist.size()<<endl;
	return 1;
}

int CInvertFile::load_base(string dbpath)
{
	ifstream infea(dbpath.c_str(), ios::in);
	string temp;
	string filename = wzp::Path::splitext(wzp::Path::basename(dbpath)).first;
	int    total;
	static int fileidx = 0;
	if(!infea)
	{
		cerr << "unable to load the input file"
			<< dbpath << endl;
		return -1;
	}
    std::getline(infea, temp);
    total = std::stoi(temp);
    if(total == 0) {
        infea.close(); return 1;
    }
    m_imglist.emplace_back(filename);
    index_t *idxlist;
    float   *valuelist;
    idxlist = new index_t[total];
    valuelist = new float[total];
    for(int j = 0; j < total; ++j) {
        index_t vwidx;
        float   vwvalue;
        std::getline(infea, temp);
        istringstream line(temp);
        line >> vwidx >> vwvalue;
        idxlist[j] = vwidx;
        valuelist[j] = vwvalue;
    }
    //l1 normalization
    float l1norm = 0;
    //#pragma omp parallel for
    for(int j = 0; j < total; j++)
    {
        l1norm += valuelist[j];
    }
    for(int j = 0; j < total; j++)
    {
        index_t vwidx;
        poolword_t item;
        item.imgidx = fileidx;
#ifdef HISTINTERSEC
        item.value = valuelist[j]*1000000/l1norm;
#else
        item.value = sqrt(valuelist[j]*1000000/l1norm);
#endif
#ifdef FFSDATASQRTNORM
        valuelist[j]=item.value;
#endif
        vwidx = idxlist[j];
        m_ifs[vwidx].push_back(item);
    }
#ifdef FFS
    m_ffs.loadone_item(idxlist,valuelist,total,fileidx);//Load one ffs item
#endif
    delete []idxlist;
    delete []valuelist;
    fileidx++;
    if(fileidx % 10000==0)
    {
        cout << GREEN << "[notice]loading the " << fileidx << "-th image" << RESET << endl;
    }
    infea.close();
    return 1;
}

vector<CScoreDesidx> CInvertFile::search_ifs(map<index_t, float>& query, string queryName)
{
	struct timeval tpstart,tpend;
	float timeuse;
	gettimeofday(&tpstart,NULL);

	cout << "[notice]retrieval query:" << queryName << endl;
#ifdef CALIDFROOT
	cal_IDFrootsim(query);	//idf weighting computing similarity
#endif
#ifdef ROOT
	cal_rootsim(query);	//without idf weighting computing similarity
#endif
#ifdef HISTINTERSEC
        cal_histintsim(query);
#endif
	vector<CScoreDesidx> filelist;
	rank_and_save(queryName,filelist);

	m_similarity.clear();

	gettimeofday(&tpend,NULL);
	timeuse=1000000*(tpend.tv_sec-tpstart.tv_sec)+
		tpend.tv_usec-tpstart.tv_usec;
	timeuse/=1000000;
	cout << GREEN << "[notice]Used Time(s): " << timeuse << RESET << endl;
	return filelist;
}

float CInvertFile::cal_selfIDFrootsim(map<index_t,float>& vwweightquery,map<index_t, float> &query)//compute similarity of two vectors
{
        map<index_t, float>::iterator iter;
	double sum=0;
        for(iter = query.begin(); iter != query.end(); ++iter)
        {
                index_t idx = iter->first;
                double vwvalue = iter->second;
		double idfweight=m_idf[idx];
		if(vwweightquery.count(idx))
		{
                	sum += idfweight *idfweight* vwweightquery[idx] * vwvalue;
		}
		else
		{
		       	continue;
		}
        }
	m_selfsimi=sum;
	cout<<"This image:self-idf-rootsimi computed over,the value is "<<sum<<endl;
        return sum;
}
int CInvertFile::queryexpand_ffs(map<index_t, float> &query,string queryname,map<index_t,float>& roivwordweight)
{
	map<index_t,float> tmpquery=query;
	cout << "[notice]retrieval query:" << queryname << endl;

	generate_renormquery(tmpquery,roivwordweight);//according to ROi of picture,renorm-L1;and query vector word weight,vw*weight
	hellinger_norm(query);//Hellinger-kernel
	// float selfsimi=cal_selfIDFrootsim(tmpquery,query);

	// ofstream oFileselfsimi("./tmp/selfIDFsimi",ios::app);
	// oFileselfsimi<<queryname<<" "<<selfsimi<<endl;
	// oFileselfsimi.close();
	
	vector<CScoreDesidx> filelist;
	filelist=search_ifs(tmpquery,queryname);//search-ifs & return most similar image list
#ifdef FFS
//	m_ffs.find_dbase(filelist[0]);//generate new query-vector
//	m_ffs.avg_query_expand(tmpquery,filelist);
/*	m_ffs.avg_queryratio_expand(query,filelist,selfsimi);
//	m_ffs.avg_expandrenorm_ratio(roivwordweight,query,filelist);
	map<index_t,float> newquery=m_ffs.get_newquery();//return new query-vec
	ret=weight_vword(newquery,roivwordweight);//codeword-weight

	string tmp="rerank/"+queryname;
	search_ifs(newquery,tmp);
	
	newquery.clear();
*/
#endif
	filelist.clear();
	return 1;
}

int CInvertFile::rank_and_save(string queryName,vector<CScoreDesidx>& filelist)
{
//	cout << "[" << queryName << "]: rank and save" << endl;
	int i,j;
	vector<CScoreDesidx> scores;

//#pragma omp parallel for
	for(i=0; i<m_imglist.size(); i++)
	{
		CScoreDesidx tmpScore;
//tmpScore.file = m_imglist[i];
		tmpScore.fileidx=i;
#ifdef SIMILARITYNORM
		tmpScore.score = m_similarity[i]*(double)1000000/m_selfsimi;
#else
		tmpScore.score = m_similarity[i];
#endif
		scores.push_back(tmpScore);
	}

	sort(scores.begin(), scores.end());
	string dstPath = m_rank_path;
	dstPath += queryName;
	dstPath += ".rank";
//	vector<int> filelist;
	save_rank_list(dstPath, scores,filelist);//save rank file

//	return filelist;
	scores.clear();
	return 0;
}

int CInvertFile::save_rank_list(string dstPath, vector<CScoreDesidx>& scores,vector<CScoreDesidx>& filelist)
{
	ofstream rnkOut(dstPath.c_str(), ios::out);
	int i=0,k=0;

	if( !rnkOut)
	{
		cerr << RED << "[error] unable to open the rnkOut file:"<< rnkOut << RESET << endl;
		return -1;	
	}

	filelist.reserve(TOPRESULT);

	string filename;
	while(k<TOPRESULT&& i<scores.size())
	{
		filename= m_imglist[scores[i].fileidx];
		if(headlist.count(filename))//remove Video Head and Black Frame from rank result 
		{
			filename.clear();			
			i++;
			continue;
		}
		else 
		{
			filelist.push_back(scores[i]);
			rnkOut<< filename<<" " << scores[i].score << endl;
			i++;
			k++;
			filename.clear();
		}
	}

	while( i<scores.size())
	{
		filename= m_imglist[scores[i].fileidx];
		if(headlist.count(filename))
		{
			filename.clear();			
			i++;
			continue;
		}
		else 
		{
			rnkOut<< filename<<" " << scores[i].score << endl;
			i++;
			filename.clear();
		}
	}
	cout<<endl;
	rnkOut.close();
	return 0;
}

int CInvertFile::cal_rootsim(map<index_t, float>& query)
{
	m_similarity.resize(m_imglist.size(),0.0);
	map<index_t, float>::iterator iter;
	for(iter = query.begin(); iter != query.end(); ++iter)
	{
		index_t idx = iter->first;
		double vwvalue = iter->second;

		vector<poolword_t> &vwvec = m_ifs[idx];
		for(int i=0; i<vwvec.size(); i++)
		{
			poolword_t term = vwvec[i];
			index_t imgidx = term.imgidx;
			double tmp=term.value;
			m_similarity[imgidx] += tmp * vwvalue;
		}
	}
	return 0;
}

int CInvertFile::cal_histintsim(map<index_t, float> &query)
{
	m_similarity.resize(m_imglist.size(),0.0);
	map<index_t, float>::iterator iter;
	for(iter = query.begin(); iter != query.end(); ++iter)
	{
		index_t idx = iter->first;
		double vwvalue = iter->second;
#ifdef STATISTICIDF
		double idfweight=m_idf[idx];
#else
		double idfweight=1;
#endif
		vector<poolword_t> &vwvec = m_ifs[idx];
		for(int i=0; i<vwvec.size(); i++)
		{
			poolword_t term = vwvec[i];
			index_t imgidx = term.imgidx;
			double tmp=(term.value<=vwvalue?term.value:vwvalue);
			m_similarity[imgidx] += tmp*idfweight;
		}
	}
	return 1;
}

int CInvertFile::cal_IDFrootsim(map<index_t, float> &query)
{
        m_similarity.resize(m_imglist.size(),0.0);
        map<index_t, float>::iterator iter;
        for(iter = query.begin(); iter != query.end(); ++iter)
        {
                index_t idx = iter->first;
                double vwvalue = iter->second;

		double idfweight=m_idf[idx];
                vector<poolword_t> &vwvec = m_ifs[idx];
                for(int i=0; i<vwvec.size(); i++)
                {
                        poolword_t term = vwvec[i];
                        index_t imgidx = term.imgidx;
                        double tmp=term.value;
                        m_similarity[imgidx] += idfweight *idfweight* tmp * vwvalue;
                }
        }
	cout<<"This image : idf-root-simi compute over\n";
        return 0;
}

int CInvertFile::asymbuild_ifs(int dictsize, string dblist)
{
        struct timeval tpstart,tpend;
        float timeuse;
        gettimeofday(&tpstart,NULL);

        cout << YELLOW << "[notice] inverted file system: building asym ifs... " << RESET << endl;

        m_dict_size = dictsize;
        if(m_dict_size <= 0)
        {
                cerr << RED << "[error] invalid dictionary size" << RESET << endl;
                return -1;
        }

        int ret;
        int i = 0;

        ret = init_ifs();

        ifstream indb(dblist.c_str(), ios::in);
        if(!indb)
        {
                cerr << "unable to load the db file:"<< dblist << endl;
                return -1;
        }

        string dbpath;
        while(getline(indb, dbpath))
        {
                ret = asymload_base(dbpath);
        }
	
        indb.close();
 	
	int picsize=m_imglist.size();
	m_T_num = 0;    //asymmetrical dissimilarity formula:numerator    w = a*num/den;
        for(int i=0;i<picsize;i++)
        {
               m_T_num+=m_T[i];
        }
//	calIdf();//compute idf weight and weighting dataset value 

        gettimeofday(&tpend,NULL);
        timeuse=1000000*(tpend.tv_sec-tpstart.tv_sec)+tpend.tv_usec-tpstart.tv_usec;
        timeuse/=1000000;
        cout << GREEN << "[notice]Used Time(s): " << timeuse << RESET << endl;
        cout << GREEN << "[success] build inverted file system success" << RESET << endl;
        cout<<"Remained pic number of Video head:"<<headlist.size()<<endl;

        return 0;
}

int CInvertFile::asymload_base(string dbpath)
{
        ifstream infea(dbpath.c_str(), ios::in);
        string temp;
        string filename;
        int    total;
        static int fileidx = 0;
        if(!infea)
        {
                cerr << "unable to load the input file" << dbpath << endl;
                return -1;
        }
        while(getline(infea, temp))
        {
                //root_sim first, then push
                istringstream line(temp);
                line >> filename >> total;
/*                if(headlist.count (filename))
                {
                        line.clear();
                        line.str("");
                        temp.clear();
                        cout<<filename<<" belong to a video head"<<endl;
                        headlist.erase(filename);
                        filename.clear();
                        continue;
                }
                else
                {
*/                      m_imglist.push_back(filename);
			double l1sum=0.0;
			if(total == 0)
                        {
				m_T.push_back(l1sum);
                                fileidx++;
                                continue;
                        }

                        //load to list;without normalizing
			for(int j=0; j<total; j++)
			{
				index_t vwidx;
				float   vwvalue;
				line >> vwidx >> vwvalue;

				l1sum+=vwvalue;
				poolword_t item;
                                item.imgidx = fileidx;
				item.value = vwvalue;
				m_ifs[vwidx].push_back(item);
			}
			m_T.push_back(l1sum);
                        fileidx++;
                        if(fileidx%100==0)
                        {
                                cout << GREEN << "[notice]loading the " << fileidx << "-th image" << RESET << endl;
                        }
                        line.clear();
                        line.str("");
                        temp.clear();
                        filename.clear();
//                }
        }
        infea.close();
        return 0;
}

int CInvertFile::asymsearch_ifs(map<index_t, float> &query, string queryName)
{
        struct timeval tpstart,tpend;
        float timeuse;
        gettimeofday(&tpstart,NULL);

        cout << "[notice]retrieval query:" << queryName << endl;
//	Idfweightquery(query);//idf weighting query value
	cal_asymdissimi_con(query);	

//	calAsymDissimw(query);
//	calAsymDissim();

	asymrank_and_save(queryName);
        m_similarity.clear();//m_similarity.swap(vector<float>() );
        
	gettimeofday(&tpend,NULL);
        timeuse=1000000*(tpend.tv_sec-tpstart.tv_sec)+tpend.tv_usec-tpstart.tv_usec;
        timeuse/=1000000;
        cout << GREEN << "[notice]Used Time(s): " << timeuse << RESET << endl;
        return 0;
}

int CInvertFile::asymrank_and_save(string queryName)
{
//	cout << "[" << queryName << "]: rank and save" << endl;
	int i,j;
	vector<CScoreDes> scores;

//#pragma omp parallel for
	for(i=0; i<m_imglist.size(); i++)
	{
		CScoreDes tmpScore;
		tmpScore.file  = m_imglist[i];
		tmpScore.score = m_similarity[i];
        	scores.push_back(tmpScore);
	}

	sort(scores.begin(), scores.end());//descending order
	string dstPath = m_rank_path;
	dstPath += queryName;
	dstPath += ".rank";
	asymsave_rank_list(dstPath, scores);
//	cout << "[" << queryName << "]: rank and save over" << endl;

	return 0;
}

int CInvertFile::asymsave_rank_list(string dstPath, vector<CScoreDes> &scores)
{
	ofstream rnkOut(dstPath.c_str(), ios::out);
	int i;

	if( !rnkOut)
	{
		cerr<<RED << "[error] unable to open the rnkOut file:"<< rnkOut << RESET << endl;
		return -1;	
	}
	for(i=0; i<scores.size(); i++)
	{
		rnkOut<<scores[i].file<<" "<< scores[i].score << endl;
	}
	rnkOut.close();
	return 0;
}

void CInvertFile::calIdf()
{
	int picsize=m_imglist.size();
        cout<<"Pic-database size : "<<picsize<<endl;
	
	ofstream oFile("./tmp/IDF.txt",ios::out);
        for (int i = 0; i < m_dict_size; i++)
        {
		int ifs_size=m_ifs[i].size();
		if(0==ifs_size)
		{
			ifs_size=1;
		}
		m_idf[i] = log2((float)picsize/(float)ifs_size);
		oFile<<m_idf[i]<<endl;
        }
	oFile.close();

	cout<<"IDF coefficient weight cal over"<<endl;
}

void CInvertFile::calE_Idf(double arfa)
{
	cout<<"Calculate E-IDF...\t Parameter:a="<<arfa<<'\n';
	int picsize=m_imglist.size();
        cout<<"Pic-database size : "<<picsize<<endl;//all document number
	ofstream oFile("./tmp/E-IDF.txt",ios::out);
        for (int i = 0; i < m_dict_size; i++)
        {
		int n_i=m_ifs[i].size();
		if(0==n_i)
		{
			n_i=1;
		}
		double tmpn_i=n_i;
		double posiexp=exp(tmpn_i/arfa);
		double negaexp=1/posiexp;
		double numerator=negaexp*(picsize-tmpn_i+posiexp-negaexp+1);
		double denominator=(posiexp-negaexp+1)*(tmpn_i+negaexp);
               	m_idf[i] = log(numerator/denominator);
		oFile<<n_i<<"\t"<<m_idf[i]<<"\t"<< posiexp<<"\t"<<negaexp<<"\t"<<numerator<<"\t"<<denominator<<endl;
        }
	oFile.close();
	cout<<"E-IDF coefficient is calculated over.\n";
}

void CInvertFile::Idfweightquery(map<index_t, float> &query)
{
	map<index_t, float>::iterator iter;
	for(iter = query.begin(); iter != query.end(); ++iter)
	{
		index_t idx = iter->first;
		iter->second*=m_idf[idx];
        }
}

int CInvertFile::cal_asymdissimi_con(map<index_t, float> &query)
{
	m_disw=500;
	int picsize=m_imglist.size();
	m_similarity.resize(picsize,0.0);

	double querysum=0;

        map<index_t, float>::iterator iter;
        for(iter = query.begin(); iter != query.end(); ++iter)
        {
                index_t idx = iter->first;
                float vwvalue = iter->second;
		querysum+=vwvalue;
 
                vector<poolword_t> &vwvec = m_ifs[idx];
                for(int i=0; i<vwvec.size(); i++)
                {
                        poolword_t term = vwvec[i];
                        index_t imgidx = term.imgidx;
                        float tmp=term.value;
                
		        if(vwvalue>tmp)
                        {
                                m_similarity[imgidx] += tmp;     //minsum
                        }
                        else
                        {
                                m_similarity[imgidx] += vwvalue;
                        }
                }
        }
	for(int i=0;i<picsize;i++)
	{
//                m_similarity[i]=(m_disw+1)*m_similarity[i]-m_disw*querysum-m_T[i]; //dissimilarity convert to similarity
		m_similarity[i]=((m_disw+1)*m_similarity[i]-m_T[i])/querysum;
	}
	return 1;
}

//weight setting(compute weight) of Query-adaptive dissimilarity
void CInvertFile::calAsymDissimw(map<index_t, float> &query)
{
	m_similarity.resize(m_imglist.size(),0.0);

        double a = 0.5;
        double den = 0;    //denominator
/*
        double num = 0;    //numerator    w = a*num/den;
	for(int i=0;i<picnum;i++) { num+=m_T[i]; }
*/	
	map<index_t, float>::iterator iter;
        for(iter = query.begin(); iter != query.end(); ++iter)
	{
		index_t idx = iter->first;
                float vwvalue = iter->second;

                vector<poolword_t> &vwvec = m_ifs[idx];
                for(int i=0; i<vwvec.size(); i++)
                {
                        poolword_t term = vwvec[i];
                        index_t imgidx = term.imgidx;
                        float tmp=term.value;
			if(vwvalue>tmp)
			{
				m_similarity[imgidx] +=tmp;	//minsum
				den+=tmp;

			}
			else
			{
				m_similarity[imgidx] +=vwvalue;
				den+=vwvalue;
			}                        
		}
	}

	if(den > 0)
        {
                m_disw = a * m_T_num / den;
        }
        else
        {
                m_disw = -1;
        }

        cout << "adapted w:" << m_disw << endl;
}

int CInvertFile::calAsymDissim()
{
	int picsize=m_imglist.size();
        double inversew;
        if(m_disw > 0)
        {
                inversew = 1 / m_disw;
        }
        else
        {
                inversew = 0;    //m_disw == -1 means m_disw == inf
        }

	for(int i=0;i<picsize;i++)
	{
		m_similarity[i]=m_similarity[i]-inversew*m_T[i];	//dissimilarity convert to similarity
	}
        return 1;
}

