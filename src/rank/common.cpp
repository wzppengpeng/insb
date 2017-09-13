#include "common.h"
#include <iostream>
#include <math.h>
#include <fstream>
using namespace std;
typedef int index_t;
extern float WEIGHTVALUE; 

int weight_vword(map<index_t,float>& query,map<index_t,float>& vwordweight)
{
	//map<index_t,float> query=vecdata;
//	cout<<queryname<<" newquery image have roi points.\n";
	if(vwordweight.empty())
	{
		cout<<"This query doesn't have ROI code word.\n";
		return 0;
	}
	map<index_t,float>::iterator iter;
	//check vwordidx waited to weight
	for(iter=vwordweight.begin();iter!=vwordweight.end();++iter)
	{
		index_t vwidx=iter->first;
		if(query.count(vwidx))
		{
			query[vwidx]=query[vwidx]*WEIGHTVALUE;
		}
	}
	return 1;
}

int vw_renorm(map<index_t,float>& query_ma,map<index_t,float>& vwordweight)
{
	map<index_t,float>::iterator iter;
	float l1sum=0;

	//check vwordidx waited to weight
	if(!(vwordweight.empty()))//vwx3-renorm
	{
		cout<<"This image vw*"<<WEIGHTVALUE<<"-renorm.\n";
		float wtvaluesum=0;
		for(iter=query_ma.begin();iter!=query_ma.end();++iter)
		{
			index_t vwidx=iter->first;
			float vwvalue=iter->second;
			if(vwordweight.count(vwidx))
			{
				wtvaluesum += vwvalue*WEIGHTVALUE;
			}
			else
			{
				wtvaluesum += vwvalue;
			}
			l1sum += vwvalue;
		}
		l1sum=wtvaluesum*wtvaluesum/l1sum;

	}
	else	//vwx1,Hellinger kernel;such as no ROIpoints
	{
		cout<<"This image Hellinger-normalize.\n";
		for(iter=query_ma.begin();iter!=query_ma.end();++iter)
		{
			float vwvalue=iter->second;
			l1sum+=vwvalue;
		}
	}
	ofstream oFile("./tmp/query.fea",ios::app);
	oFile<<query_ma.size();
	for(iter=query_ma.begin();iter!=query_ma.end();++iter)
	{
		float value=sqrt(iter->second*1000000/l1sum);
		iter->second=value;
		oFile<<" "<<iter->first<<" "<<iter->second;
	}
	oFile<<endl;
	oFile.close();
	return 1;
}

int generate_renormquery(map<index_t,float>& query_ma,map<index_t,float>& vwordweight) //generate vw*n-renorm and weight-word vector
{
	int ret=vw_renorm(query_ma,vwordweight);//according to ROi of picture,renorm-L1
	ret=weight_vword(query_ma,vwordweight);//query vector word weight
	return ret;
}


int hellinger_norm(map<index_t,float> &query_ma)
{
	map<index_t,float>::iterator iter;
	cout<<"This image Hellinger-normalize.\n";
	float l1sum=0;
	for(iter=query_ma.begin();iter!=query_ma.end();++iter)
	{
		float vwvalue=iter->second;
		l1sum+=vwvalue;
	}

	for(iter=query_ma.begin();iter!=query_ma.end();++iter)
	{
		iter->second=sqrt(iter->second*1000000/l1sum);
	}
	return 1;
}
