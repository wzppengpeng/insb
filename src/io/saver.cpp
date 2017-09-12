#include <fstream>

#include "insb/io/saver.hpp"


using namespace std;

namespace insb
{

namespace io
{

bool Saver::SaveCodeBook(const cv::Mat& codebook, const char* path) {
    ofstream ofile(path, ios::out);
    ofile << codebook.rows << '\n';
    ofile << codebook.cols << '\n';
    for(int i = 0; i < codebook.rows; ++i) {
        int j = 0;
        for(; j < codebook.cols - 1; ++j) {
            ofile << codebook.at<F>(i, j) << ' ';
        }
        ofile << codebook.at<F>(i, j) << '\n';
    }
    ofile.close();
    return true;
}

bool Saver::SaveProjectionIndexDist(const std::vector<std::pair<int, F> >& ma_index_dist,
        const std::string& save_path) {
    ofstream ofile(save_path, ios::out);
    ofile << ma_index_dist.size() << '\n';
    for(auto& p : ma_index_dist) {
        ofile << p.first << ' ' << p.second << '\n';
    }
    ofile.close();
}

} //io

} //insb