#ifndef INSB_INVERT_SEARCHER_HPP_
#define INSB_INVERT_SEARCHER_HPP_

#include "insb/task.hpp"

#include "reflection/reflection.hpp"

#include "insb/common.hpp"

#include "insb/task/rank/ranker.hpp"

/**
 * the module to generate invert index for search
 * Use the Old Code, Just Wrap it
 */
namespace insb
{

class InvertSearcher : public InsTask {

public:
    void Initial(wzp::ConfigParser* parser_ptr) override;

    void Run() override;

private:
    static ReflectionRegister(InsTask, InvertSearcher) regis_hello_invert_searcher;

private:
    // the params
    std::string m_db_dir;
    std::string m_query_dir;
    std::string m_score_dir;
    std::string m_roivw_dir;
    // the cluster num
    int m_center_num;

    // Private Members
    std::unique_ptr<rank::SearcherInterface> m_searcher;

private:
    /**
     * Private Functions
     */
     // parse the config and get the config params
    void ParseConfig();

    // prepare the environment when traning begining
    void PreEnv(); //set some value to default

};

} //insb


#endif /*INSB_INVERT_SEARCHER_HPP_*/