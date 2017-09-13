#ifndef SEARCHOBJECT_H
#define SEARCHOBJECT_H

#include <iostream>
#include <stdlib.h>

#include "search.h"

#include "config/config_parser.hpp"
//#define ASYM //change this sentence to determine whether to use asym-dissimilarity,query no need to normalize
float WEIGHTVALUE = 1;

using wzp::ConfigParser;

class SearchObject
{
public:
    SearchObject(const std::string& config) : queryfeapath("/home/zhipeng/workspace/ins/wzp_code/rank/bin/temp.roi"),
                                              searcher(0),
                                              m_parser(config)
    {
        if(!parse()) {
            cerr<<"[error] parse config file fail..."<<endl;
            exit(1);
        }
        searcher = new CSearch();
    }

    ~SearchObject() {
        if(searcher) {
            delete searcher;
            searcher = 0;
        }
    }

    void run() {
#ifdef ASYM
        searcher->asymbuild_ifs(db_path, score_path);
        searcher->asymsearch_query(queryfeapath);
#else
        searcher->build_ifs(db_path, score_path, cluster_num);
//      searcher->Loadroiidx(roivwfile);
        searcher->Loadroivwweight(roivwfile);
//      searcher->search_query(queryfeapath);
        searcher->search_queryexpand(queryfeapath);//actually ,don't expand
#endif
    }

private:
    bool parse() {
        if(!m_parser.config()) {
            return false;
        }
        if(!m_parser.get("roivwfile", roivwfile)) return false;
        // if(!m_parser.get("queryfeapath", queryfeapath)) return false;
        if(!m_parser.get("score_path", score_path)) return false;
        if(!m_parser.get("db_path", db_path)) return false;
        if(!m_parser.get("queryfeapath", queryfeapath)) return false;
        if(!m_parser.get("cluster_num", cluster_num)) return false;
        return true;
    }

private:
    string roivwfile;
    string queryfeapath;
    string score_path;
    string db_path;

    CSearch* searcher;

    ConfigParser m_parser;

    int cluster_num;
};


#endif // SEARCHOBJECT_H
