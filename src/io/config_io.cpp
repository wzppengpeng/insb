#include "insb/io/config_io.hpp"

#include "config/arg_parser.hpp"


namespace insb
{

namespace io
{

void ConfigIO::MergeArgs(wzp::ConfigParser* parser_ptr) {
    for(auto& p : wzp::ArgParser::Get()) {
        if(p.first != "conf") {
            parser_ptr->update(p.first, p.second);
        }
    }
}


} //io

} //insb