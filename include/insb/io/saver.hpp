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

    // save the projection index and dist
    static bool SaveProjectionIndexDist(const std::vector<std::pair<int, F> >& ma_index_dist,
        const std::string& save_path);

};

} //io

} //insb


#endif /*INSB_SAVER_HPP_*/