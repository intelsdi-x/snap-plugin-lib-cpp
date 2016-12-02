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

# Support travis.ci environment matrix:
TEST_TYPE="${TEST_TYPE:-$1}"
TEST_K8S="${TEST_K8S:-0}"

set -e
set -u
set -o pipefail

__dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
__proj_dir="$(dirname "$__dir")"

# shellcheck source=scripts/common.sh
. "${__dir}/common.sh"

_debug "script directory ${__dir}"
_debug "project directory ${__proj_dir}"

[[ "$TEST_TYPE" =~ ^(small|medium|large|legacy|build)$ ]] || _error "invalid TEST_TYPE (value must be 'small', 'medium', 'large', 'legacy', or 'build' recieved:${TEST_TYPE}"


if [[ $TEST_TYPE == "small" ]]; then
  if [[ -f "${__dir}/small.sh" ]]; then
    . "${__dir}/small.sh"
  else
    echo "mode: count" > profile.cov
    test_unit
  fi
elif [[ $TEST_TYPE == "medium" ]]; then
  if [[ -f "${__dir}/medium.sh" ]]; then
    . "${__dir}/medium.sh"
  else
    UNIT_TEST="go_test"
    test_unit
  fi
elif [[ $TEST_TYPE == "large" ]]; then
  if [[ "${TEST_K8S}" != "0" && -f "$__dir/large_k8s.sh" ]]; then
    . "${__dir}/large_k8s.sh"
  elif [[ -f "${__dir}/large_compose.sh" ]]; then
    . "${__dir}/large_compose.sh"
  else
    _info "No large tests."
  fi
elif [[ $TEST_TYPE == "build" ]]; then
  "${__dir}/build.sh"
fi

