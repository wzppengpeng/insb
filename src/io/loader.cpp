#include "insb/io/loader.hpp"

#include "log/log.hpp"
#include "files/files.hpp"
#include "files/path.hpp"
#include "thread/parallel_algorithm.hpp"

#include "insb/io/file_io.hpp"

using namespace std;
using wzp::print;

namespace insb
{

namespace io
{

bool Loader::LoadMatrixFromDir(const std::string& dir, Matrix<F>& datas, bool need_l2_norm) {
    //use multi thread to load datas
    //first get all filenames
    vector<string> filenames;
    CHECK(wzp::file_system::get_all_files(dir, filenames));
    vector<Matrix<F> > tmp_datas(filenames.size());
    wzp::ParallelRange(filenames.size(),
        [&dir, &filenames, &tmp_datas, need_l2_norm](size_t i) {
            auto filepath = wzp::Path::join({dir, filenames[i]});
            insb_transfer_file_into_data_matrix(filepath, tmp_datas[i], need_l2_norm);
        });
    // flat the matrix
    datas.clear();
    for(auto& mat : tmp_datas) {
        std::copy(std::begin(mat), std::end(mat), std::back_inserter(datas));
        wzp::stl_release(mat);
    }
    return true;
}


bool Loader::LoadMatrixFromPath(const std::string& path, Matrix<F>& datas,
        bool need_l2_norm) {
    datas.clear();
    insb_transfer_file_into_data_matrix(path, datas, need_l2_norm);
    return true;
}


bool Loader::LoadOpenCVMatFromPath(const std::string& path, cv::Mat* codebook) {
    // first get the row and col ox codebook(centers and dimension)
    std::ifstream ifile(path, ios::in);
    CHECK(ifile);
    int center_num, dim;
    ifile >> center_num >> dim;
    wzp::log::info("Loading The Codebook", path, "<centers>", center_num, "<dimension>", dim);
    codebook->release();
    codebook->create(center_num, dim, CV_32F);
    string line; F temp;
    for(int i = 0; i < center_num; ++i) {
        std::getline(ifile, line);
        std::istringstream is(line);
        for(int j = 0; j < dim; ++j) {
            is >> temp;
            codebook->at<F>(i, j) = temp;
        }
    }
    ifile.close();
    return true;
}

} //io

} //insb