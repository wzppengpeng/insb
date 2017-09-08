#ifndef INS_BAGGER_HPP_
#define INS_BAGGER_HPP_

/**
 * the main interface for users
 * a singleton class
 * just a wapper to update the config and call tasks
 */

#include <string>
#include <memory>


#include "config/config_parser.hpp"




namespace insb
{


class InsTask;

class InsBagger
{

public:
    //the singleton interface
    static std::shared_ptr<InsBagger> Instance(const std::string& config);
    //the point to get the instance
    static std::shared_ptr<InsBagger> GetInstance();

    //the public method
    void Run();

private:
    //delete some unused functions
    InsBagger(const InsBagger&) = delete;
    InsBagger(InsBagger&&) = delete;

    InsBagger& operator= (const InsBagger&) = delete;
    InsBagger& operator= (InsBagger&&) = delete;

    //the private constructor
    InsBagger(const std::string& config);
    //the intial fcuntion
    void Initial();

private:
    //the instance
    static std::shared_ptr<InsBagger> m_instance;

    //other members
    // the config parser
    wzp::ConfigParser m_config_parser;
    //the task ptr
    std::unique_ptr<InsTask> m_task_ptr;

};


} //insb



#endif /*INS_BAGGER_HPP_*/