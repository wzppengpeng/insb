#ifndef INSB_PROJECTOR_HPP_
#define INSB_PROJECTOR_HPP_

#include "insb/task.hpp"

#include "reflection/reflection.hpp"


#include "insb/common.hpp"

#include "insb/util/kd_tree.hpp"

#include "insb/task/projection/projector_interface.hpp"

/**
 * The module to projecting the features with codebook
 */

namespace insb
{

class Projector : public InsTask {

public:
    void Initial(wzp::ConfigParser* parser_ptr) override;

    void Run() override;

    //the destructor as there are raw pointer s
    virtual ~Projector();

private:
    // register
    static ReflectionRegister(InsTask, Projector) regis_hello_projector;

private:
    /**
     * The Params
     */
    std::string m_input_dir; //the input features need to projection

    std::string m_codebook_path; //the codebook path

    std::string m_output_dir; //the output projection file dir

    int m_need_l2_norm; //need to l2 norm?

    // the tao(need not to change in practic)
    F m_Tao_; //6250.0*2

    // the core to use, need not to change in most situations
    int m_cores;

    /**
     * Private Members
     */
    // the codebook mat
    cv::Mat m_centers;

    std::vector<KdTree> m_kd_trees; //cores kd tree, not to kasi

    // the filenames of all feature files in input dir
    std::vector<std::string> m_feature_file_names;
    std::vector<std::vector<std::string> > m_assign_file_paths;

    // the list of handle objects
    std::vector<projection::ProjectorHandler* > m_handlers;

private:
    /**
     * Private Functions
     */
     // parse the config and get the config params
    void ParseConfig();

    // prepare the environment when traning begining
    void PreEnv(); //set some value to default

    // assign the filenames
    void AssignLists();

};

} //insb

#endif /*INSB_PROJECTOR_HPP_*/