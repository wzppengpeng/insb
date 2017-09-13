#include "ffs.h"

CFfs::CFfs()
{
	m_ffstable.resize(5000000);
}

CFfs::~CFfs()
{
	m_ffstable.clear();
	m_newquery.clear();
}

int CFfs::find_dbase(index_t imgid)
{
	m_newquery.clear();
	m_newquery=m_ffstable[imgid];
	return 1;
}

map<index_t,float> CFfs::get_newquery()
{
	return m_newquery;
}

int CFfs::avg_query_expand(map<index_t,float>& query,vector<index_t>& imgidvec)
{
	m_newquery.clear();
	int imgnum = imgidvec.size();
	m_newquery = query;
	map<index_t,float>::iterator iter;

	for(int i=0;i<imgnum;i++)
	{
		map<index_t,float>& tmp=m_ffstable[imgidvec[i]];
		for(iter=tmp.begin();iter!=tmp.end();++iter)
		{
			index_t idx = iter->first;
			float value = iter->second;
			
			m_newquery[idx]+=(value/4);//the first four pic of topic-list 
		}
	}

	for(iter=m_newquery.begin();iter!=m_newquery.end();++iter)
	{
		iter->second=iter->second/2;//(imgnum+1);
	}

	return 1;
}

int CFfs::avg_queryratio_expand(map<index_t,float>& query,vector<CScoreDesidx>& imgidvec,float selfsimi)//according to similarity ratio of self and top N result,to compute new-query
{
	m_newquery.clear();
	int imgnum = imgidvec.size();
	map<index_t,float>::iterator iter;

	cout<<imgnum<<" ";
	float sum=selfsimi;
	for(int i=0;i<imgnum;i++)
	{
		sum+=imgidvec[i].score;
	}
	float iniratio=selfsimi/sum;
	cout<<"avgquery ratio:"<<iniratio<<" ";
	for(iter=query.begin();iter!=query.end();++iter)
	{
		index_t idx = iter->first;
		float value = iter->second;

		m_newquery[idx]+=(value*iniratio);
	}

	for(int i=0;i<imgnum;i++)
	{
		map<index_t,float>& tmp=m_ffstable[imgidvec[i].fileidx];
		float tmpratio=imgidvec[i].score/sum;
		cout<<tmpratio<<" ";
		for(iter=tmp.begin();iter!=tmp.end();++iter)
		{
			index_t idx = iter->first;
			float value = iter->second;
			
			m_newquery[idx]+=(value*tmpratio);
		}
	}
	cout<<endl;
	return 1;
}

int CFfs::avg_expandrenorm_ratio(map<index_t,float>& vwordweight,map<index_t,float>& query,vector<CScoreDesidx>& imgidvec)
{
	m_newquery.clear();
	int imgnum = imgidvec.size();
	map<index_t,float>::iterator iter;

	float sum=1000000;
	for(int i=0;i<imgnum;i++)
	{
		sum+=imgidvec[i].score;
	}
	float iniratio=1000000/sum;
	cout<<"avgquery ratio:"<<iniratio<<" ";
	for(iter=query.begin();iter!=query.end();++iter)
	{
		index_t idx = iter->first;
		float value = iter->second;

		m_newquery[idx]+=(value*iniratio);
	}

	for(int i=0;i<imgnum;i++)
	{
		map<index_t,float> tmp=m_ffstable[imgidvec[i].fileidx];
		
		vwrenorm_ffs(tmp,vwordweight);//L1renorm result vector,according to word of query-roi 
		
		float tmpratio=imgidvec[i].score/sum;
		cout<<tmpratio<<" ";
		for(iter=tmp.begin();iter!=tmp.end();++iter)
		{
			index_t idx = iter->first;
			float value = iter->second;
			m_newquery[idx]+=(value*tmpratio);
		}
		tmp.clear();
	}
	cout<<endl;
	return 1;
}

int CFfs::vwrenorm_ffs(map<index_t,float>& ffsitem,map<index_t,float>& vwordweight)
{
	map<index_t,float>::iterator iter;
	//check vwordidx waited to weight
	if(!vwordweight.empty())//vwx3-renorm
	{
		cout<<"This renormed-pic doesn't have ROI code word.\n";
		float l1sum=0;
		float wtvaluesum=0;
		for(iter=ffsitem.begin();iter!=ffsitem.end();++iter)
		{
			index_t vwidx=iter->first;
			float vwvalue=iter->second;
			if(vwordweight.count(vwidx))
			{
				wtvaluesum += vwvalue*3;
			}
			else
			{
				wtvaluesum += vwvalue;
			}
			l1sum += vwvalue;
		}
		l1sum=wtvaluesum*wtvaluesum/l1sum;
		l1sum/=1000000;

		for(iter=ffsitem.begin();iter!=ffsitem.end();++iter)
		{
			iter->second=sqrt(iter->second/l1sum);
		}
	}
	else	//vwx1,Hellinger kernel
	{
		float l1sum=0;
		for(iter=ffsitem.begin();iter!=ffsitem.end();++iter)
		{
			float vwvalue=iter->second;
			l1sum+=vwvalue;
		}
		l1sum/=1000000;

		for(iter=ffsitem.begin();iter!=ffsitem.end();++iter)
		{
			iter->second=sqrt(iter->second/l1sum);
		}
	}
	return 1;
}

int CFfs::loadone_item(index_t* idxlist,float* valuelist,int featuredim,int fileidx)
{
	for(int i=0;i<featuredim;i++)
	{
		m_ffstable[fileidx].insert(pair<index_t,float>(idxlist[i],valuelist[i]));
	}
	return 1;
}
