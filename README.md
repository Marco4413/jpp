# jpp

A json parsing library for C++17.

## Using the library

Add the [include](include/) directory to your include paths.

Then include the library and start using it:

```c++
// main.cpp

#define JPP_IMPLEMENTATION
#include <jpp/jpp.h>

#include <iostream>

int main(int argc, const char **argv)
{
    (void)argc;
    (void)argv;

    jpp::value json;

    jpp::value &products = json["products"];
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
```

The given example can be compiled with

```console
$ g++ -std=c++17 -o main main.cpp -Iinclude
```

The library is built with `-Wall`, `-Wextra` and `-Wpedantic`.
You can expect to be able to compile the implementation with
all warnings enabled.

As you can see the library supports stb-style implementations.
You can define `JPP_IMPLEMENTATION` before including `jpp.h`
and the header file will include all implementations for you,
so there's no need to statically link with the library.

This removes the need to integrate with the build system of
the library.

## Building

Generate Makefiles:

```console
$ premake5 gmake
```

Run make:

```console
$ make -j
```

### Running tests

Within the root directory run

```console
$ ./build/jpp-test/linux_*/*/jpp-test
```
