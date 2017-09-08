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

    // an interface to add some default value in the table of config
    template<typename T>
    inline static void AddDefaultValue(wzp::ConfigParser* parser_ptr,
        const std::string& key, const T& val) {
        T tmp;
        if(!parser_ptr->get(key, tmp)) {
            parser_ptr->update(key, std::to_string(val));
        }
    }

    //the specaial of string
    inline static void AddDefaultValue(wzp::ConfigParser* parser_ptr,
        const std::string& key, const std::string& val) {
        std::string tmp;
        if(!parser_ptr->get(key, tmp)) {
            parser_ptr->update(key, val);
        }
    }

};


} //io


} //insb


#endif /*INSB_CONFIG_IO_HPP_*/