#ifndef INSB_CONFIG_IO_HPP_
#define INSB_CONFIG_IO_HPP_

/**
 * give some interface to handle the configs
 */

#include "config/config_parser.hpp"

#include "log/log.hpp"

namespace insb
{

namespace io
{

class ConfigIO
{

public:
    // the interface to update the config parser
    // add command args into config parser
    static void MergeArgs(wzp::ConfigParser* parser_ptr);

    //the template interface to get values
    template<typename T>
    static T ReadParam(wzp::ConfigParser* parser, const std::string& key) {
        T val;
        if(!parser->get(key, val)) {
            wzp::log::fatal("Miss Key", key);
        }
        return std::move(val);
    }

};


} //io


} //insb


#endif /*INSB_CONFIG_IO_HPP_*/