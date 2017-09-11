#ifndef INSB_SAVER_HPP_
#define INSB_SAVER_HPP_


#include <opencv2/core/core.hpp>

#include "insb/common.hpp"

/**
 * a module to save diffenrent container data into disk
 */

namespace insb
{

namespace io
{

class Saver {

public:
    static bool SaveCodeBook(const cv::Mat& codebook, const char* path);

};

} //io

} //insb


#endif /*INSB_SAVER_HPP_*/