#include "insb/ins_bagger.hpp"

#include <cassert>

#include "log/log.hpp"

#include "insb/task.hpp"
#include "insb/io/config_io.hpp"
#include "insb/util/object_factory.hpp"


using namespace std;
using namespace wzp;

namespace insb
{

/**
 * The Public Interface
 */
//the instance
std::shared_ptr<InsBagger> InsBagger::m_instance(nullptr);

std::shared_ptr<InsBagger> InsBagger::Instance(const std::string& config) {
    if(!m_instance.get()) {
        m_instance = (std::shared_ptr<InsBagger>)(new InsBagger(config));
    }
    return m_instance;
}

std::shared_ptr<InsBagger> InsBagger::GetInstance() {
    CHECK(m_instance.get());
    return m_instance;
}

void InsBagger::Run() {
    //call the task ptr
    assert(m_task_ptr);
    m_task_ptr->Run();
}

/**
 * Private Funtion
 * Intialize something
 */
InsBagger::InsBagger(const std::string& config)
    : m_config_parser(config) {
        Initial();
}

void InsBagger::Initial() {
    log::log_init(log_level::Info, log_type::Console);
    CHECK(m_config_parser.config());
    //update configs
    io::ConfigIO::MergeArgs(&m_config_parser);
    m_task_ptr =  insb::ObjectFactory<InsTask>::create(io::ConfigIO::ReadParam<string>(&m_config_parser, "task"));
    m_task_ptr->Initial(&m_config_parser);
}


void InsTask::Initial(wzp::ConfigParser* parser_ptr) {
    m_parser_ptr = parser_ptr;
}

} //insb