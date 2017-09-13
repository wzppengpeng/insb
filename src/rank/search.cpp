#include "search.h"

#include "files/files.hpp"
#include "files/path.hpp"
#include "log/log.hpp"

int  CSearch::build_ifs(string dbpath, string rankdir, int cluster_num)
{
	cout<<"CSearch::build_ifs "<< endl;
	int ret;
//	m_invert_file.set_img_path(dbpath);
	m_invert_file.set_rank_path(rankdir);
//	ret = m_invert_file.load_list(imglist);
	ret = m_invert_file.build_ifs(cluster_num, dbpath);
//	m_invert_file.monitor();
	return ret;
}

int CSearch::search_query(string queryfeapath)
{
	ifstream query_in(queryfeapath.c_str(), ios::in);
	string temp;
	if( !query_in)
	{
		cerr << "error! unable to open the query_input file:"<< queryfeapath << endl;
		return -1;
	}

	//while is for each point in one image
	while(getline(query_in, temp))
	{
		if(temp.size()<5)
		{
			continue;
		}
		string queryname;
		int total;
		istringstream line(temp);
		map<index_t, float> query;

		int vwidx;
		float vwvalue;

		line >> queryname >> total;
		if(roivwordweight.count(queryname))//if(roivwidx.count(queryname))
		{
			cout<<queryname<<" image have roi points.\n";
			for (int i = 0; i < total; i++)
			{
				line >> vwidx >> vwvalue;
				query[vwidx] = vwvalue;
			}
			vw_renorm(query,roivwordweight[queryname]);//vw*weight renorm-Hellinger kernel
			weight_vword(query,roivwordweight[queryname]);//ROI-word-weight 
		}
		else
		{
			cout<<queryname<<" image doesn't have roi point!\n";
			for (int i = 0; i < total; i++)
			{
				line >> vwidx >> vwvalue;
				query[vwidx] = vwvalue;
			}
			hellinger_norm(query);//Hellinger kernel
		}
		m_invert_file.search_ifs(query, queryname);
		query.clear();
	}

	query_in.close();
	return 0;
}

int CSearch::search_queryexpand(string queryfeapath)
{

	string temp;
	vector<string> filenames;
	CHECK(wzp::file_system::get_all_files(queryfeapath, filenames));
	for(auto& filename : filenames) {
		string queryname = "/" + wzp::Path::splitext(wzp::Path::basename(filename)).first;
		string path = wzp::Path::join({queryfeapath, filename});
		ifstream ifile(path, ios::in);
		int total;
		std::getline(ifile, temp);
    	total = std::stoi(temp);
		map<index_t, float> query;

        int vwidx;
        float vwvalue;
        for (int i = 0; i < total; i++)
        {
        	std::getline(ifile, temp);
        	istringstream line(temp);
            line >> vwidx >> vwvalue;
            query[vwidx] = vwvalue;
        }
        m_invert_file.queryexpand_ffs(query, queryname, roivwordweight[queryname]);
        query.clear();
        roivwordweight.erase(queryname);
		ifile.close();
	}
	return 0;
}

int CSearch::Loadroiidx(string roivwfile)
{
	ifstream iFileroi(roivwfile.c_str(),ios::in);
	if(!iFileroi)
	{
		cout<<"Fail to open ROIvwidx file!\n";
		return 0;
	}
	string tmp;
	
	while(getline(iFileroi,tmp))
	{
		string queryname;
		int total;
		istringstream line(tmp);

                int vwidx;

                line >> queryname >> total;
		cout<<queryname<<" image ROI includes "<<total<<" points.\n";

                for (int i = 0; i < total; i++)
		{
			line >> vwidx;
			roivwidx[queryname].insert(vwidx);
		}
	}
	iFileroi.close();
	cout<<"Roi visual word idx file loaded over!\n";
	return 1;
}

int CSearch::Loadroivwweight(string roivwfile)
{
	vector<string> filenames;
	CHECK(wzp::file_system::get_all_files(roivwfile, filenames));
	string temp;
	for(auto& filename : filenames) {
		string queryname = "/" + wzp::Path::splitext(wzp::Path::basename(filename)).first;
		string path = wzp::Path::join({roivwfile, filename});
		ifstream ifile(path, ios::in);
		int total;
		std::getline(ifile, temp);
    	total = std::stoi(temp);
		int vwidx;
        float tmpweight;
        for(int i=0;i<total;i++)
        {
        	std::getline(ifile, temp);
        	istringstream line(temp);
            line >> vwidx >> tmpweight;
            roivwordweight[queryname].insert(pair<index_t,float>(vwidx,tmpweight));// ) [vwidx]=tmpweight;
        }
		ifile.close();
	}
	cout<<"Query image ROI visual word weight loaded over.\n";
	return 1;
}

int  CSearch::asymbuild_ifs(string dbpath, string rankdir)
{
        cout<<"CSearch::build_asym_ifs "<< endl;
        int ret;
//      m_invert_file.set_img_path(dbpath);
        m_invert_file.set_rank_path(rankdir);
//      ret = m_invert_file.load_list(imglist);
        ret = m_invert_file.asymbuild_ifs(1000000, dbpath);
//      m_invert_file.monitor();

        return ret;
}

int CSearch::asymsearch_query(string queryfeapath)
{
        ifstream query_in(queryfeapath.c_str(), ios::in);
        string temp;
        if( !query_in)
        {
                cerr << "error! unable to open the query_input file:"
                        << queryfeapath << endl;
                return -1;
        }

        //while is for each point in one image
        while(getline(query_in, temp))
        {
                if(temp.size()<5)
                {
                        continue;
                }
                string queryname;
                int total;
                istringstream line(temp);
                map<index_t, float> query;

                int vwidx;
                float vwvalue;

                line >> queryname >> total;
                for (int i = 0; i < total; i++)
                {
                        line >> vwidx >> vwvalue;
                        query[vwidx] = vwvalue;
                }
                m_invert_file.asymsearch_ifs(query, queryname);
                query.clear();
        }

        return 0;
}

void CSearch::calRootNorm()
{
	for(int i=0; i<iVwValVec.size(); i++)
	{
		float l1Norm = 0;
		for(int j=0; j<iVwValVec[i].size(); j++)
		{
			l1Norm += iVwValVec[i][j];
		}

		if(l1Norm > 0)
		{
			for(int j=0; j<iVwValVec[i].size(); j++)
			{
				iVwValVec[i][j] = sqrt(iVwValVec[i][j]/l1Norm);
			}
		}
	}
}

void CSearch::calL2Norm()
{
	for(int i=0; i<iVwValVec.size(); i++)
	{
		float l2Norm = 0;
		for(int j=0; j<iVwValVec[i].size(); j++)
		{
			l2Norm += iVwValVec[i][j] * iVwValVec[i][j];
		}
		l2Norm = sqrt(l2Norm);
		for(int j=0; j<iVwValVec[i].size(); j++)
		{
			iVwValVec[i][j] = iVwValVec[i][j] / l2Norm;
		}
	}
}

void CSearch::calIdf()
{
	cout<<"codebook size: "<<_cbNum<<endl;
	float *idf = new float[_cbNum];
	for (int i = 0; i < _cbNum; i++)
	{
		idf[i] = 0.1;
	}

	cout<<"pic size"<<iVwIdxVec.size()<<endl;
	for (int i = 0; i < iVwIdxVec.size(); i++)
	{
		vector<int>& idxVec = iVwIdxVec[i];
		for(int j = 0; j < idxVec.size(); j++)
		{
			idf[idxVec[j]] += 1;
			//idf[idxVec[j]] += iVwValVec[i][j];
			//idf[rand()%_cbNum] += 1;
		}
		//cout<<endl;
	}
	float total = iVwIdxVec.size();
	for (int i = 0; i < _cbNum; i++)
	{
		idf[i] = log(total/idf[i]);
	}

	for (int i = 0; i < iVwValVec.size(); i++)
	{
		for(int j=0; j < iVwValVec[i].size(); j++)
		{
			iVwValVec[i][j] *= idf[iVwIdxVec[i][j]];
		}
	}

	cout<<"idf coefficient cal over"<<endl;
	delete[] idf;
}

int CSearch::saveRankList(string dstPath, vector<CScoreDes>& scores)
{
	ofstream rnkOut(dstPath.c_str(), ios::out);

	if( !rnkOut)
	{
		cerr << "error! unable to open the rnkOut file:" 
			<< dstPath << endl;
		return -1;


	}
	for(int i=0; i<scores.size(); i++)
	{
		rnkOut<<scores[i].file<<" "<<scores[i].score<<endl;
	}
	rnkOut.close();
	return 0;
}

int CSearch::saveRankList(string dstPath, vector<CScore>& scores)
{
	ofstream rnkOut(dstPath.c_str(), ios::out);

	if( !rnkOut)
	{
		cerr << "error! unable to open the rnkOut file:" 
			<< dstPath << endl;
		return -1;
	}

	for(int i=0; i<scores.size(); i++)
	{
		rnkOut<<scores[i].file<<" "<<scores[i].score<<endl;
	}
	rnkOut.close();
	return 0;
}

float CSearch::calCosSim(vector<int>& vec1, vector<float>& val1,int index2)
{
	float sim=0;
	int p1(0),p2(0);
	float norm1(0), norm2(0);
	vector<int>& vec2=iVwIdxVec[index2];
	vector<float>& val2=iVwValVec[index2];

	for (;p1!=vec1.size()&&p2!=vec2.size();)
	{
		if (vec1[p1]<vec2[p2])
		{
			norm1 += val1[p1]*val1[p1];
			p1++;
		}
		else if (vec1[p1]>vec2[p2])
		{

			norm2 += val2[p2]*val2[p2];
			p2++;
		}
		else if (vec1[p1]==vec2[p2])
		{
			sim+=val1[p1]*val2[p2];
			norm1 += val1[p1]*val1[p1];
			norm2 += val2[p2]*val2[p2];
			p1++;
			p2++;
		}
	}
	for (int p=p1;p!=vec1.size();p++)
	{
		norm1 += val1[p]*val1[p];
	}
	for (int p=p2;p!=vec2.size();p++)
	{
		norm2 += val2[p]*val2[p];
	}

	if(sim == 0)
	{
		return 0;
	}
	sim /= (sqrt(norm1)*sqrt(norm2));
	//sim /= norm1*norm2;
	return sim;
}

float CSearch::calL2Dist(vector<int>& vec1, vector<float>& val1,int index2)
{
	float dist = 0;
	float sim = 0;
	int p1(0),p2(0);
	float norm1(0), norm2(0);
	vector<int>& vec2=iVwIdxVec[index2];
	vector<float>& val2=iVwValVec[index2];

	for (int i = 0; i < vec1.size(); ++i)
	{
		norm1 += val1[i] * val1[i];
	}
	norm1 = sqrt(norm1);

	if(norm1 > 0)
	{
		for (int i = 0; i < vec1.size(); ++i)
		{
			val1[i] /= norm1;
		}
	}


//	for (int i = 0; i < vec2.size(); ++i)
//	{
//		norm2 += val2[i]*val2[i];
//	}
//	norm2 = sqrt(norm2);


	for (;p1!=vec1.size()&&p2!=vec2.size();)
	{
		if (vec1[p1]<vec2[p2])
		{
			dist += val1[p1] * val1[p1];
			p1++;
		}
		else if (vec1[p1]>vec2[p2])
		{

			dist += val2[p2] * val2[p2];
			p2++;
		}
		else if (vec1[p1]==vec2[p2])
		{
			dist += (val1[p1] - val2[p2]) * (val1[p1] - val2[p2]);
			p1++;
			p2++;
		}
	}
	for (int p=p1;p!=vec1.size();p++)
	{
		dist += val1[p] * val1[p];
	}
	for (int p=p2;p!=vec2.size();p++)
	{
		dist += val2[p] * val2[p];
	}

	sim = 0 - dist;
	return sim;
}

float CSearch::calL2DistWithoutNorm(vector<int>& vec1, vector<float>& val1,int index2)
{
	float dist=0;
	int p1(0),p2(0);
	vector<int>& vec2=iVwIdxVec[index2];
	vector<float>& val2=iVwValVec[index2];

	for (;p1!=vec1.size()&&p2!=vec2.size();)
	{
		if (vec1[p1]<vec2[p2])
		{
			dist += val1[p1]*val1[p1];
			p1++;
		}
		else if (vec1[p1]>vec2[p2])
		{

			dist += val2[p2]*val2[p2];
			p2++;
		}
		else if (vec1[p1]==vec2[p2])
		{
			dist += (val1[p1]-val2[p2])*(val1[p1]-val2[p2]);
			p1++;
			p2++;
		}
	}
	for (int p=p1;p!=vec1.size();p++)
	{
		dist += val1[p]*val1[p];
	}
	for (int p=p2;p!=vec2.size();p++)
	{
		dist += val2[p]*val2[p];
	}

	return dist;
}

float CSearch::calL1Dist(vector<int>& vec1, vector<float>& val1,int index2)
{
	float dist=0;
	float sim=0;
	int p1(0),p2(0);
	float norm1(0), norm2(0);
	vector<int>& vec2=iVwIdxVec[index2];
	vector<float>& val2=iVwValVec[index2];

	for (int i = 0; i < vec1.size(); ++i)
	{
		norm1 += val1[i];
	}
	for (int i = 0; i < vec2.size(); ++i)
	{
		norm2 += val2[i];
	}


	for (;p1!=vec1.size()&&p2!=vec2.size();)
	{
		if (vec1[p1]<vec2[p2])
		{
			dist += (val1[p1]/norm1);			
			p1++;
		}
		else if (vec1[p1]>vec2[p2])
		{

			dist += (val2[p2]/norm2);
			p2++;
		}
		else if (vec1[p1]==vec2[p2])
		{
			dist += abs(val1[p1]/norm1-val2[p2]/norm2);
			p1++;
			p2++;
		}
	}

	for (int p=p1;p!=vec1.size();p++)
	{
		dist += (val1[p]/norm1);
	}
	for (int p=p2;p!=vec2.size();p++)
	{
		dist += (val2[p]/norm2);
	}

	sim = 0 - dist;
	return sim;
}

float CSearch::calInterSim(vector<int>& vec1, vector<float>& val1,int index2)
{
	float sim=0;
	int p1(0),p2(0);
	float norm1(0), norm2(0);
	vector<int>& vec2=iVwIdxVec[index2];
	vector<float>& val2=iVwValVec[index2];

	for (int i = 0; i < vec1.size(); ++i)
	{
		norm1 += val1[i];
	}
	for (int i = 0; i < vec2.size(); ++i)
	{
		norm2 += val2[i];
	}


	for (;p1!=vec1.size()&&p2!=vec2.size();)
	{
		if (vec1[p1]<vec2[p2])
		{
			p1++;
		}
		else if (vec1[p1]>vec2[p2])
		{

			p2++;
		}
		else if (vec1[p1]==vec2[p2])
		{
			sim += (val1[p1]/norm1 < val2[p2]/norm2)? val1[p1]/norm1 : val2[p2]/norm2;
			p1++;
			p2++;
		}
	}
	return sim;
}

//weight setting of Query-adaptive dissimilarity
void CSearch::calAsymDissimw(vector<int>& qvec, vector<float>& qval)
{
	float a = 0.5;
	float den = 0;    //denominator
	float num = 0;    //numerator    w = a*num/den;
	for(int i=0; i<iFileNameVec.size(); i++)
	{
		int p1(0),p2(0);
		vector<int>& tvec = iVwIdxVec[i];
		vector<float>& tval = iVwValVec[i];

		for (;p1!=qvec.size() && p2!=tvec.size();)
		{
			if (qvec[p1] < tvec[p2])
			{
				p1++;
			}
			else if (qvec[p1] > tvec[p2])
			{
				num += tval[p2];
				p2++;
			}
			else if (qvec[p1] == tvec[p2])
			{
				num += tval[p2];

				if( qval[p1] >= tval[p2] )
				{
					den += tval[p2];
				}
				else
				{
					den += qval[p1];
				}
				p1++;
				p2++;
			}
		}
		for (int p = p2; p != tvec.size(); p++)
		{
			num += tval[p];
		}
	}

	if(den > 0)
	{
		m_disw = a * num / den;
	}
	else
	{
		m_disw = -1;
	}

	cout << "adapted w:" << m_disw << endl;
}


//Asymmetrical dissimilarity ; the larger,the dissimilar
float CSearch::calAsymDissim(vector<int>& qvec, vector<float>& qval,int tindex)
{

	double inversew;
	double dissim = 0.0;        //dissim = tsum - minsum;
	double tsum = 0.0;
	double minsum = 0.0;
	float sim = 0.0;
	int p1(0),p2(0);
	vector<int>& tvec = iVwIdxVec[tindex];
	vector<float>& tval = iVwValVec[tindex];

	if(m_disw > 0)
	{
		inversew = 1 / m_disw;
	}
	else
	{
		inversew = 0;    //m_disw == -1 means m_disw == inf
	}

	for (;p1!=qvec.size() && p2!=tvec.size();)
	{
		if (qvec[p1] < tvec[p2])
		{
			//dissim += qval[p1] * 100;
			p1++;
		}
		else if (qvec[p1] > tvec[p2])
		{
			tsum += tval[p2];
			//dissim += tval[p2] * 100 * inversew ;
			p2++;
		}
		else if (qvec[p1] == tvec[p2])
		{
			tsum += tval[p2];
			if( qval[p1] >= tval[p2] )
			{
				minsum += tval[p2];
			}
			else
			{
				minsum += qval[p1];
			}
			p1++;
			p2++;
		}
	}
	for (int p = p2; p != tvec.size(); p++)
	{
		tsum += tval[p];
	}

	dissim = inversew * tsum - minsum;
        sim = 0 - dissim;
	return sim;
}
