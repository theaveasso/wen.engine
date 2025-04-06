#include "wen_io.hpp"

std::string
wen_read_file(std::string_view filename) {
    std::ifstream file(filename.data());
    assert(file.is_open());
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    return buffer.str();
}