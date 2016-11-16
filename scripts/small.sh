#!/bin/bash

set -e
set -u
set -o pipefail

__dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
__proj_dir="$(dirname "$__dir")"
__proj_name="snap-plugin-lib-cpp"

# shellcheck source=scripts/common.sh
. "${__dir}/common.sh"

TEST_TYPE="${TEST_TYPE:-"small"}"

_debug "building googletest"
make -C "${__proj_dir}/googletest"

_debug "building snap plugin lib"
export SNAPLIB_DIR="${__proj_dir}/lib"
mkdir -p "${SNAPLIB_DIR}"
pushd "${__proj_dir}"
"${__proj_dir}/autogen.sh"
"${__proj_dir}/configure" --prefix="${SNAPLIB_DIR}"
make -C "${__proj_dir}"
make -C "${__proj_dir}" install
popd

_debug "running tests"
make -C "${__proj_dir}/test"

