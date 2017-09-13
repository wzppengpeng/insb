#ifndef INSB_INVERT_SEARCH_RANKER_HPP_
#define INSB_INVERT_SEARCH_RANKER_HPP_

/**
 * the interface of searcher
 */
#include <string>


namespace insb
{

namespace rank
{

class SearcherInterface {

public:
    // the create functions
    // 3 path and 1 center num
    static SearcherInterface* create(const std::string& db_dir,
        const std::string& query_dir, const std::string& score_dir,
        const std::string& rowvw_dir, int center_num);


    virtual ~SearcherInterface() {}

    // the run function
    virtual void Run() = 0;

};

} //rank

} //insb



#endif /*INSB_INVERT_SEARCH_RANKER_HPP_*/