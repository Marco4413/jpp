#define JPP_IMPLEMENTATION
#include <jpp/jpp.h>

#include <iostream>

#include "common/args.cpp"
#include "common/files.cpp"

int main(int argc, const char **argv)
{
    shift_args(argc, argv, nullptr); // program_name
    const std::filesystem::path file_path = shift_args(argc, argv, "examples/parse_from_file.json");

    const std::string json_content = read_file(file_path);

    jpp::parser::error error;
    jpp::value value = jpp::parse(json_content, &error);
    if (error.is_set()) {
        std::cerr << error.to_string(file_path.string()) << std::endl;
        return 1;
    }

    std::cout << jpp::serialize(value) << std::endl;

    return 0;
}
