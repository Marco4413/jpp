#define JPP_IMPLEMENTATION
#include <jpp/jpp.h>

#include <iostream>

constexpr jpp::string_view json_content = R"(
{
  /* name of the user to display in the UI */
  "displayName": "John Doe",
  "username": "john.doe",
  "credits": 100e3,
  "credits": 1e3,
  // additional data
  "extra":
}
)";

int main(void)
{
    jpp::parser::options options;
    // keep the parsed value on error
    options.allow_partial_parsing = true;
    options.allow_comments        = true;
    options.allow_duplicate_keys  = true;
    // allow commas on the last element of arrays/objects
    options.allow_trailing_commas = true;

    jpp::parser::error error;
    jpp::value json = jpp::parse(json_content, &error, options);
    if (error.is_set()) {
        std::cerr << error.to_string() << std::endl;
    }

    std::cout << jpp::serialize(json) << std::endl;

    return 0;
}
