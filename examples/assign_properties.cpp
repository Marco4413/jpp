#define JPP_IMPLEMENTATION
#include <jpp/jpp.h>

#include <iostream>

int main(void)
{
    jpp::value json;

    jpp::value &products = json["products"];
    /* with a mutable jpp::value you can access
       and create/change properties on the fly */
    products[0]["id"]      = "P001";
    products[0]["name"]    = "Laptop";
    products[0]["price"]   = 999.99;
    products[0]["inStock"] = true;
    products[0]["image"]   = jpp::null;

    products[1]["id"]      = "P002";
    products[1]["name"]    = "Mouse";
    products[1]["price"]   = 29.99;
    products[1]["inStock"] = true;
    products[1]["image"]   = jpp::null;

    products[2]["id"]      = "P003";
    products[2]["name"]    = "Keyboard";
    products[2]["price"]   = 100;
    products[2]["inStock"] = false;
    products[2]["image"]   = jpp::null;

    std::cout << jpp::serialize(json) << std::endl;

    return 0;
}
