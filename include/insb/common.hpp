#ifndef INSB_COMMON_HPP_
#define INSB_COMMON_HPP_


#include "function/help_function.hpp"

namespace insb
{


/**
 * some typedefs
 */
//the float def
typedef float F;


//the simple print
#ifndef PRINT
#define PRINT(...) wzp::print(##__VA_ARGS__)
#endif


} //insb



#endif //INSB_COMMON_HPP_