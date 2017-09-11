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

} //io

} //insb