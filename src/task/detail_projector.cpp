#include "insb/task/projection/projector_interface.hpp"

#include "log/log.hpp"
#include "files/path.hpp"

#include "insb/io/loader.hpp"
#include "insb/io/saver.hpp"

#include "insb/util/kd_tree.hpp"

using namespace std;
using wzp::print;
using wzp::log;

namespace insb
{

namespace projection
{

/**
 * the class to process the projection work
 */
class ProjectorWorker : public ProjectorHandler {

public:
    ProjectorWorker(F Tao_, bool need_l2_norm, const std::string& output_dir)
        : m_Tao_(Tao_), m_need_l2_norm(need_l2_norm), m_output_dir(output_dir)
    {}

    void LoadCodeBook(cv::Mat* codebook, insb::KdTree* kd_tree) override;

    void ProcessBow(const std::vector<std::string>& filepaths) override;


private:
    F m_Tao_;
    bool m_need_l2_norm;

    std::string m_output_dir;

    /**
     * private members
     */
    cv::Mat* m_codebook;

    Matrix<F> m_descriptor; //each load one file

    KdTree* m_kd_tree;

    int m_center_num, m_dimension;

    std::vector<F> m_soft_freq;


private:
    /**
     * private functions
     */
    // load the key point
    void LoadKeyPoint(const std::string& filepath);

    // bow one file
    void ProcessOne();

    // save the result
    void Save(const std::string& filepath); //save into out dir

};

/**
 * The creat function of interface
 */
ProjectorHandler* ProjectorHandler::create(F Tao_, bool need_l2_norm, const std::string& output_dir) {
    return new ProjectorWorker(Tao_, need_l2_norm, output_dir);
}

/**
 * the worker part
 */
void ProjectorWorker::LoadCodeBook(cv::Mat* codebook, insb::KdTree* kd_tree) {
    m_codebook = codebook;
    m_kd_tree = kd_tree;
    m_center_num = m_codebook->rows;
    m_dimension = m_codebook->cols;
    m_soft_freq.clear();
    m_soft_freq.resize(m_center_num, 0.);
}

void ProjectorWorker::LoadKeyPoint(const std::string& filepath) {
    io::Loader::LoadMatrixFromPath(filepath, m_descriptor, m_need_l2_norm);
}


void ProjectorWorker::ProcessOne() {
    for(int i = 0; i < m_center_num; ++i) m_soft_freq[i] = 0.0;
    // for each feature in this file
    for(int i = 0; i < static_cast<int>(m_descriptor.size()); ++i) {
        auto& query = m_descriptor[i];
        auto near_pair = m_kd_tree->KnnSearch(query, 8, 128);
        auto& indices = near_pair.first;
        auto& dist = near_pair.second;
        dist[0] = exp(-1.0 * dist[0] / m_Tao_);
        dist[1] = exp(-1.0 * dist[1] / m_Tao_);
        dist[2] = exp(-1.0 * dist[2] / m_Tao_);
        F l1Norm = dist[0] + dist[1] + dist[2];
        dist[0] /= l1Norm;
        dist[1] /= l1Norm;
        dist[2] /= l1Norm;
        m_soft_freq[indices[0]]+=dist[0];
        m_soft_freq[indices[1]]+=dist[1];
        m_soft_freq[indices[2]]+=dist[2];
    }
}

void ProjectorWorker::Save(const std::string& filepath) {
    vector<std::pair<int, F> > ma_index_dist; ma_index_dist.reserve(m_center_num);
    for(int i = 0; i < m_center_num; ++i) {
        if(m_soft_freq[i] > 0.) {
            ma_index_dist.emplace_back(std::make_pair(i, m_soft_freq[i]));
        }
    }
    insb::io::Saver::SaveProjectionIndexDist(ma_index_dist, filepath);
}

void ProjectorWorker::ProcessBow(const std::vector<std::string>& filepaths) {
    for(auto& filepath : filepaths) {
        string file_base_name = wzp::Path::splitext(wzp::Path::basename(filepath)).first;
        string out_path = wzp::Path::join({m_output_dir, file_base_name + ".proj"});
        LoadKeyPoint(filepath);
        ProcessOne();
        Save(out_path);
    }
}

} //projection

} //insb


