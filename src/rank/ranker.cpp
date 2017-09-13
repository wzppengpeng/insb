#include "insb/task/rank/ranker.hpp"

#include <memory>

#include "search.h"

float WEIGHTVALUE = 1;

using namespace std;

namespace insb
{

namespace rank
{


// The Implement
class SearcherImple : public SearcherInterface {

public:
    SearcherImple(const std::string& db_dir,
        const std::string& query_dir, const std::string& score_dir,
        const std::string& rowvw_dir, int center_num)
    : m_db_dir(db_dir), m_query_dir(query_dir), m_score_dir(score_dir),
      m_roivw_dir(rowvw_dir), m_center_num(center_num), m_searcher(new CSearch())
    {}

    void Run() override;

private:
    // the params
    std::string m_db_dir;
    std::string m_query_dir;
    std::string m_score_dir;
    std::string m_roivw_dir;
    // the cluster num
    int m_center_num;

private:
    /**
     * members
     */
    std::unique_ptr<CSearch> m_searcher;

};


/**
 * the create function
 */
SearcherInterface* SearcherInterface::create(const std::string& db_dir,
        const std::string& query_dir, const std::string& score_dir,
        const std::string& rowvw_dir, int center_num) {
    return new SearcherImple(db_dir, query_dir, score_dir, rowvw_dir, center_num);
}

void SearcherImple::Run() {
    m_searcher->build_ifs(m_db_dir, m_score_dir, m_center_num);
    m_searcher->Loadroivwweight(m_roivw_dir);
    m_searcher->search_queryexpand(m_query_dir);
}

} //rank

} //insb