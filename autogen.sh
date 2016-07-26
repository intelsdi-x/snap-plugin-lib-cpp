#! /bin/bash

mkdir -p include
curl -L -o include/json.hpp https://raw.githubusercontent.com/nlohmann/json/7f4dd5d6088b7f1cb95acb31d8c4f31503193b64/src/json.hpp

autoreconf -fvi
