#ifndef INSB_HELLO_WORLD_HPP_
#define INSB_HELLO_WORLD_HPP_


#include "insb/task.hpp"

#include "reflection/reflection.hpp"


namespace insb
{

class Hello : public InsTask
{

public:

    void Initial(wzp::ConfigParser* parser_ptr) override;

    void Run() override;

private:
    // register
    static ReflectionRegister(InsTask, Hello) regis_hello_world;

};

} //insb



#endif /*INSB_HELLO_WORLD_HPP_*/