#include "insb/task/kmeans.hpp"

#include <unordered_set>

#include "log/log.hpp"
#include "util/timer.hpp"
#include "files/files.hpp"

#include "insb/util/insb_math.hpp"
#include "insb/util/opencv_wrapper.hpp"

#include "insb/io/config_io.hpp"
#include "insb/io/loader.hpp"
#include "insb/io/saver.hpp"

#include "omp.h"

using namespace std;

using wzp::log;
using wzp::print;

/**
 * The Kmeans++ algorithm
 */
namespace insb
{

namespace algorithm
{

class KmeansPPImpl {

public:
    KmeansPPImpl(Matrix<F>& train_set, int train_num);

    // init the centers
    void InitialCenters(cv::Mat* m_centers);

private:
    Matrix<F>& m_train_set;
    int m_train_num;

    cv::Mat m_temp_centers;

private:
    /**
     * Private Functions
     */
    // build the temp kd tree
    void BuildTempKdTree(cv::Mat* m_centers, int i, insb::KdTree& kd_tree);

    // Get the distance vector
    F ComputeDistanceVector(insb::KdTree& kd_tree, vector<F>& distance);

    int ChooseOneClusterIndex(cv::Mat* m_centers, int i, insb::KdTree& kd_tree);

};

KmeansPPImpl::KmeansPPImpl(Matrix<F>& train_set, int train_num)
    : m_train_set(train_set), m_train_num(train_num)
    {}

void KmeansPPImpl::InitialCenters(cv::Mat* m_centers) {
    int first = math::insb_random_int<int>(0, m_train_num - 1);
    insb::SetCVMatRow(m_centers, 0, m_train_set[first]);
    KdTree kd_tree;
    for(int i = 1; i < m_centers->rows; ++i) {
        int r = ChooseOneClusterIndex(m_centers, i, kd_tree);
        insb::SetCVMatRow(m_centers, i, m_train_set[r]);
    }
}

void KmeansPPImpl::BuildTempKdTree(cv::Mat* m_centers, int i, insb::KdTree& kd_tree) {
    m_temp_centers.release();
    m_temp_centers.create(i, m_centers->cols, CV_32F);
    for(int r = 0; r < i; ++r) {
        insb::SetCVMatRow<F>(&m_temp_centers, r, *m_centers, r);
    }
    kd_tree.ReBuild(m_temp_centers, insb::KdTree::AKM);
}

F KmeansPPImpl::ComputeDistanceVector(insb::KdTree& kd_tree, vector<F>& distance) {
    distance.resize(m_train_set.size(), 0.);
    F dis_all = 0.;
    for(size_t i = 0; i < m_train_set.size(); ++i) {
        auto& fea_vec = m_train_set[i];
        auto dis_pair = kd_tree.KnnSearch(fea_vec, 1);
        distance[i] = dis_pair.second[0];
        dis_all += distance[i];
    }
    return dis_all;
}

int KmeansPPImpl::ChooseOneClusterIndex(cv::Mat* m_centers, int i, insb::KdTree& kd_tree) {
    BuildTempKdTree(m_centers, i, kd_tree);
    vector<F> dx;
    F sum_dx = ComputeDistanceVector(kd_tree, dx);
    F rand_dx = math::insb_random_real<F>(0, sum_dx);
    int j = 0;
    for(; j < m_train_num - 1; ++j) {
        rand_dx -= dx[j];
        if(rand_dx <= 0.) break;
    }
    return j;
}

} //algorithm

} //insb


/**
 * the part of KmeansClusterTask Implement
 */
namespace insb
{

// reflection
ReflectionRegister(InsTask, KmeansCluster) regis_hello_kmeans_cluster("kmeans_cluster");


void KmeansCluster::Initial(wzp::ConfigParser* parser_ptr) {
    InsTask::Initial(parser_ptr);
    ParseConfig();
    PreEnv();
}


void KmeansCluster::ParseConfig() {
    io::ConfigIO::AddDefaultValue(m_parser_ptr, "kmeans_type", string("kmeans"));
    io::ConfigIO::AddDefaultValue(m_parser_ptr, "iter_min", 120);
    io::ConfigIO::AddDefaultValue(m_parser_ptr, "iter_max", 300);
    io::ConfigIO::AddDefaultValue(m_parser_ptr, "iter_save", 80);
    io::ConfigIO::AddDefaultValue(m_parser_ptr, "need_l2_norm", 0);
    m_cluster_num = io::ConfigIO::ReadParam<int>(m_parser_ptr, "cluster_num"); //k
    m_dimension = io::ConfigIO::ReadParam<int>(m_parser_ptr, "dimension");
    log::info("cluster number", m_cluster_num, "dimension", m_dimension);
    auto m_type = io::ConfigIO::ReadParam<string>(m_parser_ptr, "kmeans_type");
    m_codebook_save_dir = io::ConfigIO::ReadParam<string>(m_parser_ptr, "save_dir");
    log::info("CodeBook Save Dir", m_codebook_save_dir);
    log::info("kmeans type", m_type);
    if(m_type == "kmeans") {
        m_kmeans_type = KMEANS;
    } else {
        m_kmeans_type = KMEANSPP;
    }
    m_iter_min = io::ConfigIO::ReadParam<int>(m_parser_ptr, "iter_min");
    m_iter_max = io::ConfigIO::ReadParam<int>(m_parser_ptr, "iter_max");
    m_save_iter = io::ConfigIO::ReadParam<int>(m_parser_ptr, "iter_save");
    m_need_l2_norm = io::ConfigIO::ReadParam<int>(m_parser_ptr, "need_l2_norm");
    log::info("Stop Flag <min>", m_iter_min, "<max>", m_iter_max, "Save Iter Num", m_save_iter);
}

void KmeansCluster::PreEnv() {
    m_best_run = 0;
    m_best_rss = -1.0;
    m_rss.resize(3, 0.0); m_rss[0] = -1.0;
    // clear the dir
    wzp::file_system::clear_dir(m_codebook_save_dir);
    m_centers.create(m_cluster_num, m_dimension, CV_32F);
    log::info("Create Memory For Cluster Done...");
    // load datas
    LoadTrainSet();
    if(m_kmeans_type == KMEANS) {
        InitClusterCenter();
    } else {
        InitClusterCenterPP();
    }
}

void KmeansCluster::LoadTrainSet() {
    auto dir = io::ConfigIO::ReadParam<string>(m_parser_ptr, "train_set_dir");
    log::info("Load Traning Data From", dir);
    CHECK(io::Loader::LoadMatrixFromDir(dir, m_train_set, m_need_l2_norm == 1));
    CHECK(m_train_set[0].size() == static_cast<size_t>(m_dimension));
    // set the train set num
    m_train_num = m_train_set.size();
    log::info("Traning Set Loaded... Total Train Set Number", m_train_num);
}

void KmeansCluster::InitClusterCenter() {
    unordered_set<int> already_in;
    for(int i = 0; i < m_cluster_num;) {
        int r = math::insb_random_int<int>(0, m_train_num - 1);
        if(already_in.count(r)) continue;
        // set the col
        insb::SetCVMatRow(&m_centers, i, m_train_set[r]);
        already_in.emplace(r);
        ++i;
    }
    m_kd_tree.Release();
    log::info("Centers Has Been Built");
}

void KmeansCluster::InitClusterCenterPP() {
    algorithm::KmeansPPImpl kmeans_pp(m_train_set, m_train_num);
    kmeans_pp.InitialCenters(&m_centers);
}

void KmeansCluster::ClearEnv() {

}

void KmeansCluster::ClearUpdate(cv::Mat* update, std::vector<int>* update_num) {
    for(int i = 0; i < m_cluster_num; ++i) {
        for(int j = 0; j < m_dimension; ++j) {
            update->at<F>(i, j) = 0.;
        }
        *(update_num->begin() + i) = 0;
    }
}

bool KmeansCluster::Converge(int run) const {
    double diffrss0, diffrss1;
    diffrss0 = abs(m_rss[1] - m_rss[0]);
    diffrss1 = abs(m_rss[2] - m_rss[1]);
    if(run <= m_iter_min) {
        return false;    //at least run for m_iter_min times
    } else if(diffrss0 >= diffrss1 * 0.9 && diffrss0 <= diffrss1 * 1.1) {
        return true;    //converge
    } else if(run > m_iter_max) {
        return true;    //at most run for itermax times
    } else if(diffrss0 < m_rss[0]*0.0003) {
        return true;
    } else {
        return false;   //not converge
    }
}

void KmeansCluster::ReCalculateCenters(const cv::Mat& update, const std::vector<int>& update_num) {
    for(int i = 0; i < m_cluster_num; ++i) {
        if(update_num[i] == 0) {
            int r = math::insb_random_int<int>(0, m_train_num - 1);
            SetCVMatRow(&m_centers, i, m_train_set[r]);
        } else {
            SetCVMatRowWithOperation<F, std::function<F(F)> >(&m_centers, i, update, i,
                [&update_num, i] (F val) {
                    return val / static_cast<F>(update_num[i]);
            });
        }
    }
}

void KmeansCluster::SaveCodeBook(int run) const {
    if(run < m_save_iter) return;
    char cb_path[1024];
    string run_out_path(m_codebook_save_dir + "/run_");
    sprintf(cb_path, "%s%d%s", run_out_path.c_str(), run, ".txt");
    io::Saver::SaveCodeBook(m_centers, cb_path);
}

void KmeansCluster::Run() {
    // build the kd tree
    m_kd_tree.Build(m_centers, KdTree::AKM);
    wzp::Timer timer;
    cv::Mat update(m_cluster_num, m_dimension, CV_32F);
    std::vector<int> update_num(m_cluster_num, 0);
    int run = 0;
    while(!Converge(run)) {
        timer.reset();
        ClearUpdate(&update, &update_num);
        m_rss[2] = m_rss[1];
        m_rss[1] = m_rss[0];
        m_rss[0] = 0.0;
#pragma omp parallel for
        for(int i = 0; i < m_train_num; ++i) {
            auto near_pair = m_kd_tree.KnnSearch(m_train_set[i], 1);
            vector<int>& indice = near_pair.first;
            vector<F>& dist = near_pair.second;
#pragma omp critical
            {
                insb::AddCVMatRow(&update, indice[0], m_train_set[i]);
                ++update_num[indice[0]];
                m_rss[0] += dist[0];
            }
        }
        // update the new centers
        ReCalculateCenters(update, update_num);
        // rebuild the kd tree
        m_kd_tree.ReBuild(m_centers, KdTree::AKM);
        ++run;
        // save result
        SaveCodeBook(run);
        // update the best rss
        if(m_best_rss < 0. || m_rss[0] < m_best_rss) {
            m_best_run = run;
            m_best_rss = m_rss[0];
        }
        log::info("Run Iter", run, "Done... Use Time (s)", timer.elapsed_seconds(),
            "The Rss", m_rss[0]);
    }
    log::info("Kmeans Done, Best Run Time <run>", m_best_run, "<rss>", m_best_rss);
    update.release();
}

} //insb