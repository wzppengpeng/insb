#ifndef INSB_KD_TREE_HPP_
#define INSB_KD_TREE_HPP_

#include <memory>

#include <utility>

#include <opencv2/core/core.hpp>

#include "insb/common.hpp"

/**
 * the wrapper of opencv flann index of kd tree
 */
namespace insb
{

class KdTreeImple;

//the interface of KdTree with linear params
class KdTree {

public:
    // the enum of kd tree type
    enum KnnType {
        AKM,
        LINEAR
    };

    // the create funtion
    KdTree();

    // the interface to build KDTree
    void Build(const cv::Mat& dataset, KnnType type);

    // the release interface
    void Release();

    // the rebuild easy function
    void ReBuild(const cv::Mat& dataset, KnnType type);

    // the search interaface
    std::pair<std::vector<int>, std::vector<F> > KnnSearch(const std::vector<F>& data,
        int knn);

private:
    std::shared_ptr<KdTreeImple> m_kd_tree;

};


} //insb


#endif /*INSB_KD_TREE_HPP_*/