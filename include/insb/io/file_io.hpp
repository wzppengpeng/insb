#ifndef INSB_FILE_IO_HPP_
#define INSB_FILE_IO_HPP_

/**
 * Some Functions To Handle a data file or feature file
 */
#include <fstream>
#include <sstream>

#include "insb/common.hpp"

#include "log/log.hpp"

namespace insb
{

namespace io
{

// the function to split a string line into vector
template<typename T>
void insb_transfer_string_to_data_vector(const std::string& line, std::vector<T>& data) {
    T val;
    std::istringstream is(line);
    while(is >> val) {
        data.emplace_back(val);
    }
}

// the function to tranfer file data into matrix
template<typename T>
void insb_transfer_file_into_data_matrix(const std::string& file, Matrix<T>& datas) {
    std::ifstream ifile(file, std::ios::in);
    CHECK(ifile);
    std::vector<T> row;
    std::string line;
    while(std::getline(ifile, line)) {
        insb_transfer_string_to_data_vector(line, row);
        datas.emplace_back(row);
        row.clear();;
    }
    ifile.close();
}


} //io

} //insb



#endif /*INSB_FILE_IO_HPP_*/