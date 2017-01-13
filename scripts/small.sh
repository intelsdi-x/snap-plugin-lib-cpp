#!/bin/bash
# File managed by pluginsync

# http://www.apache.org/licenses/LICENSE-2.0.txt
#
#
# Copyright 2016 Intel Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

set -e
set -u
set -o pipefail

__dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
__proj_dir="$(dirname "$__dir")"
__proj_name="snap-plugin-lib-cpp"

# shellcheck source=scripts/common.sh
. "${__dir}/common.sh"

TEST_TYPE="${TEST_TYPE:-"small"}"
COV_ARGS="${COV_ARGS:-"-g -fprofile-arcs -ftest-coverage --coverage -fPIC -DPIC -lgcov -O0"}"

_debug "building googletest"
make -C "${__proj_dir}/googletest"

_debug "building Snap plugin lib"
export SNAPLIB_DIR="${__proj_dir}/lib"
mkdir -p "${SNAPLIB_DIR}"
pushd "${__proj_dir}"
"${__proj_dir}/autogen.sh"
"${__proj_dir}/configure" CPPFLAGS="--std=c++0x ${COV_ARGS}" LDFLAGS="${COV_ARGS}"  --prefix="${SNAPLIB_DIR}"
make -C "${__proj_dir}"
make -C "${__proj_dir}" install
popd

_debug "running tests"
make -C "${__proj_dir}/test"

