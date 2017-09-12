#ifndef INSB_PROJECTOR_INTERFACE_HPP_
#define INSB_PROJECTOR_INTERFACE_HPP_


/**
 * The interface to handle projectiong works
 */

#include <memory>

#include "insb/common.hpp"

namespace cv
{
class Mat;
} //cv

namespace insb
{

class KdTree;

namespace projection
{

class ProjectorHandler {

public:
    // constructor to create the point of object
    static ProjectorHandler* create(F Tao_, bool need_l2_norm, const std::string& output_dir);

    // deconstrcutor
    virtual ~ProjectorHandler() {}

    // Get Code Book And Kdtree
    virtual void LoadCodeBook(cv::Mat* codebook, insb::KdTree* kd_tree) = 0;

    // process the bow
    virtual void ProcessBow(const std::vector<std::string>& filepaths) = 0;

};


} //projection

} //insb

#endif /*INSB_PROJECTOR_INTERFACE_HPP_*/