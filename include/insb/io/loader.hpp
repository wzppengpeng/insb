#ifndef INSB_LOADER_HPP_
#define INSB_LOADER_HPP_

#include "insb/common.hpp"

#include "opencv2/core/core.hpp"


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
    static bool LoadMatrixFromDir(const std::string& dir, Matrix<F>& datas,
        bool need_l2_norm = false);

    // get a file data into 2-D matrix
    static bool LoadMatrixFromPath(const std::string& path, Matrix<F>& datas,
        bool need_l2_norm = false);

    // load the (opencv Mat)codebook from a specific path
    static bool LoadOpenCVMatFromPath(const std::string& path, cv::Mat* codebook);

};


} //io

} //insb



#endif /*INSB_LOADER_HPP_*/