#include "insb/task/invert_search.hpp"


#include "log/log.hpp"


#include "insb/io/config_io.hpp"

using namespace std;

namespace insb
{


// reflection
ReflectionRegister(InsTask, InvertSearcher) regis_hello_invert_searcher("invert_search");

void InvertSearcher::Initial(wzp::ConfigParser* parser_ptr) {
    InsTask::Initial(parser_ptr);
    ParseConfig();
    PreEnv();
}

void InvertSearcher::ParseConfig() {
    m_db_dir =  io::ConfigIO::ReadParam<string>(m_parser_ptr, "db_dir");
    m_query_dir = io::ConfigIO::ReadParam<string>(m_parser_ptr, "query_dir");
    m_score_dir = io::ConfigIO::ReadParam<string>(m_parser_ptr, "score_dir");
    io::ConfigIO::AddDefaultValue(m_parser_ptr, "roivw_dir", m_query_dir);
    m_roivw_dir = io::ConfigIO::ReadParam<string>(m_parser_ptr, "roivw_dir");
    m_center_num = io::ConfigIO::ReadParam<int>(m_parser_ptr, "center_num");
}

void InvertSearcher::PreEnv() {
    m_searcher = std::unique_ptr<rank::SearcherInterface>(rank::SearcherInterface::create(
        m_db_dir, m_query_dir, m_score_dir, m_roivw_dir, m_center_num));
}

void InvertSearcher::Run() {
    m_searcher->Run();
}

} //insb