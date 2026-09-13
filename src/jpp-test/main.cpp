#include <filesystem>
#include <fstream>
#include <iostream>
#include <string_view>

#ifndef JPP_TEST_STATIC_LINK
#  define JPP_IMPLEMENTATION
#endif // JPP_TEST_STATIC_LINK
#include "jpp/jpp.h"

jpp::string read_file(const std::filesystem::path &path);

bool run_parsing_test(const std::filesystem::path &file_path);
bool run_parsing_tests(const std::filesystem::path &base_path);

int main(int argc, const char **argv)
{
    (void)argc;
    (void)argv;

    if (!run_parsing_tests("tests/JSONTestSuite")) return 1;
    return 0;
}

jpp::string read_file(const std::filesystem::path &path)
{
    std::ifstream file(path);

    jpp::string contents;
    contents.resize(std::filesystem::file_size(path));

    file.read(contents.data(), contents.size());
    return contents;
}

#define FMT_LIGHT_BLUE "\x1B[38;2;173;216;230m"
#define FMT_ORANGE     "\x1B[38;2;255;165;0m"
#define FMT_RED        "\x1B[38;2;255;0;0m"
#define FMT_GREEN      "\x1B[38;2;0;128;0m"
#define FMT_RESET      "\x1B[0m"

constexpr std::string_view RUNNING_PREFIX = FMT_LIGHT_BLUE "[running]: " FMT_RESET;
constexpr std::string_view ERROR_PREFIX   = FMT_ORANGE     "[error  ]: " FMT_RESET;
constexpr std::string_view FAILED_PREFIX  = FMT_RED        "[failed ]: " FMT_RESET;
constexpr std::string_view SUCCESS_PREFIX = FMT_GREEN      "[success]: " FMT_RESET;

bool run_parsing_test(const std::filesystem::path &file_path)
{
    const auto file_name = file_path.filename();
    if (file_name.empty()) return false;

    bool ignored      = file_name.c_str()[0] == 'i';
    bool should_error = file_name.c_str()[0] == 'n';

    std::cout << RUNNING_PREFIX << file_path.string() << std::endl;

    jpp::parser::options options;
    options.allow_duplicate_keys = true;
    options.max_depth            = 500;

    jpp::parser::error error;
    jpp::value value = jpp::parse(read_file(file_path), &error, options);
    if (error.is_set()) {
        std::cout << ERROR_PREFIX << error.to_string(file_path.string()) << std::endl;
    }

    bool success = ignored || (should_error == error.is_set());
    if (success) {
        std::cout << SUCCESS_PREFIX << file_path.string() << std::endl;
    } else {
        std::cerr << FAILED_PREFIX << file_path.string() << std::endl;
    }
    return success;
}

bool run_parsing_tests(const std::filesystem::path &base_path)
{
    size_t total_tests  = 0;
    size_t failed_tests = 0;

    for (const auto &file_path : std::filesystem::directory_iterator(base_path / "test_parsing")) {
        total_tests += 1;
        if (!run_parsing_test(file_path)) {
            failed_tests += 1;
        }
    }

    if (failed_tests > 0) {
        std::cerr << FAILED_PREFIX << failed_tests << '/' << total_tests << " tests failed" << std::endl;
        return false;
    }

    std::cout << SUCCESS_PREFIX << "all tests were successful" << std::endl;
    return true;
}
