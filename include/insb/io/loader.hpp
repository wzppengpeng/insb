#ifndef INSB_LOADER_HPP_
#define INSB_LOADER_HPP_

#include "insb/common.hpp"

/**
 * a module to load different types data into contaniers
 */


namespace insb
{

namespace io
{

class Loader {

public:
    //first get all files from a dir, then get each small matrix into a big one
    static bool LoadMatrixFromDir(const std::string& dir, Matrix<F>& datas);

};


} //io

} //insb



#endif /*INSB_LOADER_HPP_*/