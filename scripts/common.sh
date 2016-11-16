set -e
set -u
set -o pipefail

LOG_LEVEL="${LOG_LEVEL:-6}"
NO_COLOR="${NO_COLOR:-}"

trap_exitcode() {
  exit $?
}

trap trap_exitcode SIGINT

_fmt () {
  local color_debug="\x1b[35m"
  local color_info="\x1b[32m"
  local color_notice="\x1b[34m"
  local color_warning="\x1b[33m"
  local color_error="\x1b[31m"
  local colorvar=color_$1

  local color="${!colorvar:-$color_error}"
  local color_reset="\x1b[0m"
  if [ "${NO_COLOR}" = "true" ] || [[ "${TERM:-}" != "xterm"* ]] || [ -t 1 ]; then
    # Don't use colors on pipes or non-recognized terminals
    color=""; color_reset=""
  fi
  echo -e "$(date -u +"%Y-%m-%d %H:%M:%S UTC") ${color}$(printf "[%9s]" "${1}")${color_reset}";
}


_debug ()   { [ "${LOG_LEVEL}" -ge 7 ] && echo "$(_fmt debug) ${*}" 1>&2 || true; }
_info ()    { [ "${LOG_LEVEL}" -ge 6 ] && echo "$(_fmt info) ${*}" 1>&2 || true; }
_notice ()  { [ "${LOG_LEVEL}" -ge 5 ] && echo "$(_fmt notice) ${*}" 1>&2 || true; }
_warning () { [ "${LOG_LEVEL}" -ge 4 ] && echo "$(_fmt warning) ${*}" 1>&2 || true; }
_error ()   { [ "${LOG_LEVEL}" -ge 3 ] && echo "$(_fmt error) ${*}" 1>&2 || true; exit 1; }


