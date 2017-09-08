#ifndef INSB_TASK_FACTORY_HPP_
#define INSB_TASK_FACTORY_HPP_

/**
 * The Wrapper of Reflections
 */

#include "reflection/reflection.hpp"

namespace insb
{

template<typename Objective>
class ObjectFactory
{

public:
    inline static std::unique_ptr<Objective> create(const std::string& object_name) {
        return wzp::Reflection<Objective>::create_unique(object_name);
    }

};

}


#endif /*INSB_TASK_FACTORY_HPP_*/