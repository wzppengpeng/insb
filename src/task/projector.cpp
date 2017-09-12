#include "insb/task/projector.hpp"

#include <cassert>

#include <thread>

#include "log/log.hpp"
#include "files/files.hpp"
#include "files/path.hpp"
#include "util/timer.hpp"

#include "thread/parallel_algorithm.hpp"


#include "insb/io/config_io.hpp"
#include "insb/io/loader.hpp"

using namespace std;

using wzp::log;
using wzp::print;

// std::thread::hardware_concurrency()

namespace insb
{

ReflectionRegister(InsTask, Projector) regis_hello_projector("projection");

void Projector::Initial(wzp::ConfigParser* parser_ptr) {
    InsTask::Initial(parser_ptr);
    ParseConfig();
    PreEnv();
}

Projector::~Projector() {
    for(size_t i = 0; i < m_handlers.size(); ++i) {
        delete m_handlers[i];
        m_handlers[i] = nullptr;
    }
}

void Projector::ParseConfig() {
    io::ConfigIO::AddDefaultValue(m_parser_ptr, "need_l2_norm", 0);
    io::ConfigIO::AddDefaultValue(m_parser_ptr, "Tao", 6250.0*2);
    m_input_dir = io::ConfigIO::ReadParam<string>(m_parser_ptr, "input_dir");
    m_codebook_path = io::ConfigIO::ReadParam<string>(m_parser_ptr, "codebook_path");
    m_output_dir = io::ConfigIO::ReadParam<string>(m_parser_ptr, "output_dir");
    m_need_l2_norm = io::ConfigIO::ReadParam<int>(m_parser_ptr, "need_l2_norm");
    m_Tao_ = io::ConfigIO::ReadParam<F>(m_parser_ptr, "Tao");
    log::info("The Input Feature Dir", m_input_dir);
    log::info("The Codebook Path", m_codebook_path);
    log::info("The Projection Output Dir", m_output_dir);
    log::info("The Norm Parameter", m_Tao_);
    if(m_need_l2_norm == 1) log::info("Use L2 Normalize For Input Data");
}

void Projector::PreEnv() {
    CHECK(wzp::file_system::get_all_files(m_input_dir, m_feature_file_names));
    log::info("Data File Number", m_feature_file_names.size());
    wzp::file_system::clear_dir(m_output_dir);
    m_cores = std::min(static_cast<int>(m_feature_file_names.size()),
        static_cast<int>(std::thread::hardware_concurrency()));
    m_assign_file_paths.resize(m_cores);
    AssignLists();
    for(int i = 0; i < m_cores; ++i)
        m_handlers.emplace_back(projection::ProjectorHandler::create(m_Tao_, m_need_l2_norm == 1, m_output_dir));
    m_kd_trees.resize(m_cores);
    // Load the codebook
    io::Loader::LoadOpenCVMatFromPath(m_codebook_path, &m_centers);
    // build kd tree
    for(auto& kd_tree : m_kd_trees) {
        kd_tree.Build(m_centers, KdTree::AKM, 8);
    }
}


void Projector::AssignLists() {
    assert(m_assign_file_paths.size() == m_cores);
    for(int i = 0; i < static_cast<int>(m_feature_file_names.size()); ++i) {
        int bin = i % m_cores;
        m_assign_file_paths[bin].emplace_back(wzp::Path::join({m_input_dir, m_feature_file_names[i]}));
    }
}


void Projector::Run() {
    wzp::Timer timer;
    log::info("Begin To Projecting Datas...");
    wzp::ParallelRange(m_cores,
        [this](int i) {
            // assign to each handler
            m_handlers[i]->LoadCodeBook(&m_centers, &(m_kd_trees[i]));
            m_handlers[i]->ProcessBow(m_assign_file_paths[i]);
        });
    log::info("Projecting Done..., Use Time (s)", timer.elapsed_seconds());
}

} //insb


