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
GTESTLIB_DIR="$(cd "${__proj_dir}/googletest" && pwd)"
GTEST_VER="${GTEST_VER:-release-1.8.0}"
GTEST_REPO="${GTEST_REPO:-https://github.com/google/googletest.git}"
GIT="${GIT:-git}"

# shellcheck source=scripts/common.sh
. "${__dir}/common.sh"

_debug "getting googletest source"
##make -C ${GTESTLIB_DIR} sources
if [ -d "${GTESTLIB_DIR}/testing" ]; then
	_debug "repository working copy already available"
	exit 0
fi
"${GIT}" clone --branch ${GTEST_VER} -c advice.detachedHead=false "${GTEST_REPO}" "${GTESTLIB_DIR}/testing"

