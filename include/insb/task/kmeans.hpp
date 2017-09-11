#ifndef INSB_KMEANS_HPP_
#define INSB_KMEANS_HPP_


#include "insb/task.hpp"

#include <mutex>

#include "reflection/reflection.hpp"

#include "insb/common.hpp"
#include "insb/util/kd_tree.hpp"

/**
 * The module to implement kmeans cluster
 * input data sets and output the codebook
 */

namespace insb
{


class KmeansCluster : public InsTask {

public:

    void Initial(wzp::ConfigParser* parser_ptr) override;

    void Run() override;

private:
    // register
    static ReflectionRegister(InsTask, KmeansCluster) regis_hello_kmeans_cluster;

    // the mutex to change the update matrix
    mutable std::mutex m_mut;

private:
    // the type of init type
    enum KmeansType {
        KMEANS,
        KMEANSPP
    };
    /**
     * Person Members
     */
    // params
    int m_cluster_num;
    int m_dimension;
    int m_train_num; //the tran set size

    int m_iter_min;
    int m_iter_max;
    int m_save_iter;

    std::string m_codebook_save_dir;

    KmeansType m_kmeans_type;

    ////////////////////

    Matrix<F> m_train_set;

    KdTree m_kd_tree; //the kd tree to get the knn search result

    cv::Mat m_centers;

    // the vector save the rss
    std::vector<double> m_rss;
    // the best time and best
    int m_best_run;
    double m_best_rss;

private:
    /**
     * Private Functions
     */
    // parse the config and get the config params
    void ParseConfig();

    // prepare the environment when traning begining
    void PreEnv(); //set some value to default

    // load train set
    void LoadTrainSet();

    // intial the cluster center default
    void InitClusterCenter();

    void InitClusterCenterPP(); //kmeans++

    // clear environment
    void ClearEnv();

    // clear the cv mat and update indice
    void ClearUpdate(cv::Mat* update, std::vector<int>* update_num);

    // check if is the end
    bool Converge(int run) const;

    // re-calculate the centers
    void ReCalculateCenters(const cv::Mat& update, const std::vector<int>& update_num);

    // save the result
    void SaveCodeBook(int run) const;


};


} //insb


#endif /*INSB_KMEANS_HPP_*/