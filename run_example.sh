#!/usr/bin/env bash

set -e

cc="${cc:-c++}"
example_executable='./example_runner'

if [ "$#" -lt 1 ]; then
    echo 'no example provided'
    exit 1
fi

set -x

example="$1"
shift

"$cc" -Wall -Wextra -Wpedantic -Werror -std=c++17 -o "$example_executable" "$example" -Iinclude
"$example_executable" $@
