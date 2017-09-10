#ifndef INSB_OPENCV_WRAPPER_HPP_
#define INSB_OPENCV_WRAPPER_HPP_

#include <cassert>
#include <vector>

#include <opencv2/opencv.hpp>

/**
 * some function wrapper to call opencv functions
 */

namespace insb
{

// the function to set the Mat row
template<typename T>
void SetCVMatRow(cv::Mat* mat, int i, const std::vector<T>& vec) {
    assert(mat->cols == static_cast<int>(vec.size()));
    for(int j = 0; j < mat->cols; ++j) {
        mat->at<T>(i, j) = vec[j];
    }
}

// the mat type
template<typename T>
void SetCVMatRow(cv::Mat* mat, int i, const cv::Mat& raw_mat, int r) {
    assert(mat->cols == raw_mat.cols);
    for(int j = 0; j < mat->cols; ++j) {
        mat->at<T>(i, j) = raw_mat.at<T>(r, j);
    }
}



// the function to set the mat row with an operation
template<typename T, typename Fun>
void SetCVMatRowWithOperation(cv::Mat* mat, int i,
    const std::vector<T>& vec, Fun&& fun) {
    assert(mat->cols == static_cast<int>(vec.size()));
    for(int j = 0; j < mat->cols; ++j) {
        mat->at<T>(i, j) = fun(vec[j]);
    }
}

// the mat type
template<typename T, typename Fun>
void SetCVMatRowWithOperation(cv::Mat* mat, int i,
    const cv::Mat& raw_mat, int r, Fun&& fun) {
    assert(mat->cols == raw_mat.cols);
    for(int j = 0; j < mat->cols; ++j) {
        mat->at<T>(i, j) = fun(raw_mat.at<T>(r, j));
    }
}


} //insb




#endif /*INSB_OPENCV_WRAPPER_HPP_*/