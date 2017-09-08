#include "insb/io/loader.hpp"

#include "log/log.hpp"
#include "files/files.hpp"
#include "files/path.hpp"
#include "thread/parallel_algorithm.hpp"

#include "insb/io/file_io.hpp"

using namespace std;

namespace insb
{

namespace io
{

bool Loader::LoadMatrixFromDir(const std::string& dir, Matrix<F>& datas) {
    //use multi thread to load datas
    //first get all filenames
    vector<string> filenames;
    CHECK(wzp::file_system::get_all_files(dir, filenames));
    vector<Matrix<F> > tmp_datas(filenames.size());
    wzp::ParallelRange(filenames.size(),
        [&dir, &filenames, &tmp_datas](size_t i) {
            auto filepath = wzp::Path::join({dir, filenames[i]});
            insb_transfer_file_into_data_matrix(filepath, tmp_datas[i]);
        });
    // flat the matrix
    datas.clear();
    for(auto& mat : tmp_datas) {
        std::copy(std::begin(mat), std::end(mat), std::back_inserter(datas));
        wzp::stl_release(mat);
    }
    return true;
}


} //io

} //insb