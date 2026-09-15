#ifndef _FILES_CPP
#define _FILES_CPP

#include <filesystem>
#include <fstream>
#include <string>

static std::string read_file(const std::filesystem::path &file_path)
{
    std::ifstream file(file_path);
    size_t file_size = std::filesystem::file_size(file_path);

    std::string content;
    content.resize(file_size);

    file.read(content.data(), file_size);
    return content;
}

#endif // _FILES_CPP
