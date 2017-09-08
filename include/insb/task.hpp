#ifndef INSB_TASK_HPP_
#define INSB_TASK_HPP_


/**
 * The Interface Of Tasks
 */

//declare of wzp::ConfigParser
namespace wzp {
    class ConfigParser;
}

namespace insb
{

class InsTask
{

public:
    virtual ~InsTask() {}

    //the interface to get a config parser
    virtual void Initial(wzp::ConfigParser* parser_ptr);

    //the interface to run
    virtual void Run() = 0;

protected:
    wzp::ConfigParser* m_parser_ptr;

};


} //insb


#endif /*INSB_TASK_HPP_*/