#include "insb/task/hello_world.hpp"

#include "log/log.hpp"

using namespace std;

namespace insb
{

// reflection
ReflectionRegister(InsTask, Hello) regis_hello_world("hello");


void Hello::Initial(wzp::ConfigParser* parser_ptr) {
    InsTask::Initial(parser_ptr);
    wzp::log::info("I Have Been Intial");
}


void Hello::Run() {
    wzp::log::info("Hello World");
}

} //insb