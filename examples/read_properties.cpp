#define JPP_IMPLEMENTATION
#include <jpp/jpp.h>

#include <chrono>
#include <iostream>

int main(void)
{
    jpp::value json;

    json["username"] = "john.doe";
    json["password"] = "password";
    json["metadata"]["lastLogin"] = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();

    /* you can dereference jpp::value to get a
       const reference which allows you to access
       values without creating/changing data. if
       a value does not exist, null is returned */
    const jpp::value &username    = (*json)["username"];
    const jpp::value &last_login  = (*json)["metadata"]["lastLogin"];
    const jpp::value &first_group = (*json)["metadata"]["group"][0];

    std::cout << jpp::serialize(json) << std::endl;
    std::cout << "username:    " << jpp::serialize(username)    << std::endl;
    std::cout << "last_login:  " << jpp::serialize(last_login)  << std::endl;
    std::cout << "first_group: " << jpp::serialize(first_group) << std::endl;

    return 0;
}
