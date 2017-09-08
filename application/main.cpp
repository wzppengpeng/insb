
#include "config/arg_parser.hpp"

#include "insb/ins_bagger.hpp"

using namespace std;
using namespace wzp;

int main(int argc, char *argv[])
{
    wzp::ArgParser::parse(argc, argv);
    auto worker = insb::InsBagger::Instance(wzp::ArgParser::get("conf"));
    worker->Run();
    return 0;
}