#include "insb/util/kd_tree.hpp"

#include "opencv2/flann/flann.hpp"


using std::vector;

namespace insb
{

/**
 * the implement class of kd tree
 */
class KdTreeImple {

public:
    // constructors
    KdTreeImple();

    // the interface to build KDTree
    void Build(const cv::Mat& dataset, KdTree::KnnType type);

    // the release interface
    void Release();

    // the search interaface
    std::pair<std::vector<int>, std::vector<F> > KnnSearch(const std::vector<F>& data,
        int knn);

private:
    cv::flann::Index m_index;

};


/**
 * the implemention
 */
KdTree::KdTree() : m_kd_tree(new KdTreeImple())
{
}

void KdTree::Build(const cv::Mat& dataset, KdTree::KnnType type) {
    m_kd_tree->Build(dataset, type);
}

void KdTree::Release() {
    m_kd_tree->Release();
}

void KdTree::ReBuild(const cv::Mat& dataset, KdTree::KnnType type) {
    Release();
    Build(dataset, type);
}

std::pair<std::vector<int>, std::vector<F> > KdTree::KnnSearch(const std::vector<F>& data,
        int knn) {
    return std::move(m_kd_tree->KnnSearch(data, knn));
}

/**
 * the implemention of imple class
 */
KdTreeImple::KdTreeImple() : m_index() {}

void KdTreeImple::Build(const cv::Mat& dataset, KdTree::KnnType type) {
    if(type == KdTree::AKM) {
        m_index.build(dataset, cv::flann::KDTreeIndexParams(4));
    } else {
        m_index.build(dataset, cv::flann::LinearIndexParams());
    }
}

void KdTreeImple::Release() {
    m_index.release();
}

std::pair<std::vector<int>, std::vector<F> > KdTreeImple::KnnSearch(const std::vector<F>& data,
        int knn) {
    vector<int> indice(knn);
    vector<F> dist(knn);
    m_index.knnSearch(data, indice, dist, knn, cv::flann::SearchParams(512));
    return std::move(std::make_pair(indice, dist));
}

} //insb