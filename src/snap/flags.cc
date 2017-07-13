/*
http://www.apache.org/licenses/LICENSE-2.0.txt
Copyright 2017 Intel Corporation
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#include <iostream>
#include <list>
#include <map>

#include <boost/any.hpp>
#include <json.hpp>
#include <spdlog/spdlog.h>

#include "snap/flags.h"

// Set default option values
// Default global flags:
#define LISTEN_PORT ""
#define LISTEN_ADDR "127.0.0.1"
#define LOG_LEVEL 2
#define CERT_PATH ""
#define KEY_PATH ""
#define ROOT_CERT_PATHS ""
#define CIPHER_SUITES ""
#define STAND_ALONE_PORT 8182
#define MAX_COLLECT_DURATION 10
#define MAX_METRICS_BUFFER 0

// Default hidden flags:
#define OPTIONS_FILE "options.cfg"

namespace po = boost::program_options;
namespace spd = spdlog;
using json = nlohmann::json;


namespace Plugin {
  namespace JsonHelpers {
    nlohmann::json generateJsonFromCommandLine(char* cmd_line);
    nlohmann::json removeUnusedFlags(nlohmann::json j);
    void generateCommandLineFromJson(nlohmann::json jsonArgs, char** argv, int index);
    std::map<std::string, std::string> _pretty_flags = {{"TLSEnabled", "tls"},
                                                        {"LogLevel", "log-level"},
                                                        {"CertPath", "cert-path"},
                                                        {"KeyPath", "key-path"},
                                                        {"RootCertPaths", "root-cert-paths"},
                                                        {"Pprof", "pprof"}};
    bool isFlag(std::string flag);
    char* copyStringStream(std::stringstream& src);
    std::shared_ptr<spd::logger> _logger = spdlog::stderr_logger_mt("jsonhelpers"); 

  }
}

bool Plugin::JsonHelpers::isFlag(std::string flag) {
  bool retVal = false;
  if (_pretty_flags.find(flag) != _pretty_flags.end()) {
    retVal = true;
  }
  return retVal;
}

// Group of options allowed only on command line
int Plugin::Flags::addDefaultCommandFlags() {
    try {
        _command.add_options()
            ("help,h", "Display available command-line options")
            ("version,v", "Print the version");
        return 0;
    } 
    catch (std::exception &e) {
        _logger->error(e.what());
        return 1;
    }
}

int Plugin::Flags::addDefaultGlobalFlags() {
    try {
        // Declare group of options
        // allowed both on command line and in config file
        _global.add_options()
            ("config", po::value<std::string>()->composing(), "Config to use in JSON format")
            ("port", po::value<std::string>(&_listen_port)->default_value(LISTEN_PORT), "Port GRPC will listen on")
            ("addr", po::value<std::string>(&_listen_addr)->default_value(LISTEN_ADDR), "addr GRPC will listen on")
            ("log-level", po::value<int>(&_log_level)->default_value(LOG_LEVEL), 
                "0:Panic 1:Fatal 2:Error 3:Warn 4:Info 5:Debug")
            ("pprof", "Enable pprof")
            ("tls", "Enable TLS")
            ("cert-path", po::value<std::string>(&_cert_path)->default_value(CERT_PATH), "Necessary to provide when TLS enabled")
            ("key-path", po::value<std::string>(&_key_path)->default_value(KEY_PATH), "Necessary to provide when TLS enabled")
            ("root-cert-paths", po::value<std::string>(&_root_cert_paths)->default_value(ROOT_CERT_PATHS), "Necessary to provide when TLS enabled.")
            ("stand-alone", "Enable stand-alone plugin")
            ("stand-alone-port", po::value<int>(&_stand_alone_port)->default_value(STAND_ALONE_PORT),
                "Specify http port when stand-alone is set")
            ("max-collect-duration", po::value<int>(&_max_collect_duration)->default_value(MAX_COLLECT_DURATION),
                "In seconds, sets the maximum duration (always greater than 0s) between collections before metrics are sent")
            ("max-metrics-buffer", po::value<int64_t>(&_max_metrics_buffer)->default_value(MAX_METRICS_BUFFER),
                "Maximum number of metrics the plugin is buffering before sending metrics");
        _config_file.add(_global);
        return 0;
    }
    catch (std::exception &e) {
        _logger->error(e.what());
        return 1;
    }
}

int Plugin::Flags::addDefaultHiddenFlags() {
    try {
        _hidden.add_options()
                ("options-file", po::value<string>(&_options_file)->default_value(OPTIONS_FILE),
                    "name of file for options configuration");
        return 0;
    }
    catch (std::exception &e) {
        _logger->error(e.what());
        return 1;
    }
}

int Plugin::Flags::SetDefaultFlags() {
    try {
        if (addDefaultCommandFlags() != 0) return 1;
        if (addDefaultGlobalFlags() != 0) return 1;
        if (addDefaultHiddenFlags() != 0) return 1;
        return 0;
    }
    catch (std::exception &e) {
        _logger->error(e.what());
        return 1;
    }
}

int Plugin::Flags::addBoolFlag(const char *optionName, const char *description,
                                Plugin::Flags::FlagLevel flagLevel) {
    try {
        switch (flagLevel) {
        case Command:
            _command.add_options()
                    (optionName, description);
            break;
        case Global:
            _global.add_options()
                    (optionName, description);
            break;
        case Hidden:
            _hidden.add_options()
                    (optionName, description);
            break;
        case Custom:
            _additional.add_options()
                    (optionName, description);
            break;
        default:
            _logger->warn("Flag level not supported");
            return 1;
        }
        return 0;
    } 
    catch (std::exception &e) {
        _logger->error(e.what());
        return 1;
    }
}
int Plugin::Flags::addIntFlag(const char *optionName, const char *description,
                                FlagLevel flagLevel) {
    try {
        switch (flagLevel) {
        case Command:
            _command.add_options()
                    (optionName, po::value<int>()->composing(), description);
            break;
        case Global:
            _global.add_options()
                    (optionName, po::value<int>()->composing(), description);
            break;
        case Hidden:
            _hidden.add_options()
                    (optionName, po::value<int>()->composing(), description);
            break;
        case Custom:
            _additional.add_options()
                    (optionName, po::value<int>()->composing(), description);
            break;
        default:
            _logger->warn("Flag level not supported");
            return 1;
        }
        return 0;
    } 
    catch (std::exception &e) {
        _logger->error(e.what());
        return 1;
    }
}

int Plugin::Flags::addStringFlag(const char *optionName, const char *description,
                                FlagLevel flagLevel) {
                            try {
        switch (flagLevel) {
        case Command:
            _command.add_options()
                    (optionName, po::value<std::string>()->composing(), description);
            break;
        case Global:
            _global.add_options()
                    (optionName, po::value<std::string>()->composing(), description);
            break;
        case Hidden:
            _hidden.add_options()
                    (optionName, po::value<std::string>()->composing(), description);
            break;
        case Custom:
            _additional.add_options()
                    (optionName, po::value<std::string>()->composing(), description);
            break;
        default:
            _logger->warn("Flag level not supported");
            return 1;
        }
        return 0;
    } 
    catch (std::exception &e) {
        _logger->error(e.what());
        return 1;
    }
}

// Declare group of options allowed only on command line
int Plugin::Flags::AddFlag(const char * optionName, const char * description, 
                            FlagType flagType, Plugin::Flags::FlagLevel flagLevel) {
    try {
        switch (flagType) {
        case Bool:
            return addBoolFlag(optionName, description, flagLevel);
        case Int:
            return addIntFlag(optionName, description, flagLevel);
        case String:
            return addStringFlag(optionName, description, flagLevel);
        default:
            _logger->warn("Flag type not supported");
            return 1;
        }
        return 0;
    } 
    catch (std::exception &e) {
        _logger->error(e.what());
        return 1;
    }
}

int Plugin::Flags::setVisibleFlags() {
    try {
        _visible.add(_command).add(_global).add(_additional);
        return 0;
    }
    catch (std::exception &e) {
        _logger->error(e.what());
        return 1;
    }
}

int Plugin::Flags::setCommandLineFlags() {
    try {
        _command_line.add(_command).add(_global).add(_additional).add(_hidden);
        return 0;
    }
    catch (std::exception &e) {
        _logger->error(e.what());
        return 1;
    }
}

int Plugin::Flags::setConfigFileFlags() {
    try {
        _config_file.add(_global).add(_additional);
        return 0;
    }
    catch (std::exception &e) {
        _logger->error(e.what());
        return 1;
    }
}

int Plugin::Flags::SetCombinedFlags() {
    try {
        if (setVisibleFlags() != 0) return 1;
        if (setCommandLineFlags() != 0) return 1;
        if (setConfigFileFlags() != 0) return 1;
        return 0;
    }
    catch (std::exception &e) {
        _logger->error(e.what());
        return 1;
    }
}

int Plugin::Flags::parseCommandLineFlags(const int &argc, char **argv) {
    try {
      if (parsejsonFlags(argc, argv) == 0) {
        return 0;
      }
    }
    catch (std::exception &e) {
      _logger->debug(e.what());
    }
    try {
        po::store(po::command_line_parser(argc, argv).options(_command_line).run(), _flags);
        po::notify(_flags);

        if (_flags.count("help")) {
            exit(helpFlagCalled());
        }
        return 0;
    }
    catch (std::exception &e) {
        _logger->error(e.what());
        return 1;
    }
}

/* 
 * The Go plugin lib assumes that the first item command line arguement might
 * be JSON. This function checks for JSON and does the following:
 *
 *   If it is JSON it parses it out and runs the parseCommandLineFlags() function
 *   Otherwise it logs a debug message with the error, and returns failure.
 */
int Plugin::Flags::parsejsonFlags(const int &argc, char** argv) {
  if (argc <= 1) {
    return 1;
  }
  int retVal = 1;

  json jsonArgs;

  try {
    jsonArgs = Plugin::JsonHelpers::generateJsonFromCommandLine(argv[1]);
  }
  catch (std::exception &e) {
    _logger->debug(e.what());
    return retVal;
  }

  jsonArgs = Plugin::JsonHelpers::removeUnusedFlags(jsonArgs);

  int newArgc = argc + (jsonArgs.size() * 2) - 1;
  char **newArgv = new char*[newArgc];
  int i = 0;

  newArgv[i] = argv[i];
  for (i = 1; i < argc - 1; i++){
    newArgv[i] = argv[i + 1];
  }

  Plugin::JsonHelpers::generateCommandLineFromJson(jsonArgs, newArgv, i);

  retVal = parseCommandLineFlags(newArgc, newArgv);

  return retVal;
}

json Plugin::JsonHelpers::generateJsonFromCommandLine(char* cmd_line){
  std::stringstream ss;
  json j;
  ss << cmd_line;
  ss >> j;
  return j;
}

json Plugin::JsonHelpers::removeUnusedFlags(json j) {
  std::list<std::string> notFlag;
  for (json::iterator it = j.begin(); it != j.end(); it++) {
    if (!isFlag(it.key())) {
      std::stringstream error;
      error << "Key provided but not supported: " << it.key() << std::endl;
      _logger->error(error.str());
      notFlag.push_back(it.key());
    }
  }

  for (auto flag : notFlag) {
    j.erase(flag);
  }
  return j;
}

void Plugin::JsonHelpers::generateCommandLineFromJson(json jsonArgs, char** argv, int index) {
  std::stringstream temp_ss;
  for (json::iterator it = jsonArgs.begin(); it != jsonArgs.end(); it++) {
    temp_ss = std::stringstream();
    temp_ss << "--";
    temp_ss << _pretty_flags[it.key()];
    argv[index++] = Plugin::JsonHelpers::copyStringStream(temp_ss);
    temp_ss = std::stringstream();
    temp_ss << it.value();
    argv[index++] = copyStringStream(temp_ss);
  }
}

char* Plugin::JsonHelpers::copyStringStream(std::stringstream& src) {
  std::string temp_str(src.str());
  temp_str.erase(std::remove(temp_str.begin(), temp_str.end(), '"'), temp_str.end());
  char* dest = new char[temp_str.size() + 1];
  memcpy(dest, temp_str.c_str(), temp_str.size() + 1);
  return dest;
}

int Plugin::Flags::parseConfigFileFlags(std::string filePathAndName /*=""*/) {
    try {
        ifstream ifs;
        filePathAndName.empty() ? ifs.open(_options_file.c_str()) :
            ifs.open(filePathAndName.c_str());
        if (ifs) {
            po::store(parse_config_file(ifs, _config_file), _flags);
            po::notify(_flags);
        }
        return 0;
    }
    catch (std::exception &e) {
        _logger->error(e.what());
        return 1;
    }
}

int Plugin::Flags::ParseFlags(const int &argc, char **argv,
                                std::string filePathAndName /*=""*/) {
    try {
        if (parseCommandLineFlags(argc, argv) != 0) return 1;
        if (parseConfigFileFlags(filePathAndName) != 0) return 1;
        return 0;
    }
    catch (std::exception &e) {
        _logger->error(e.what());
        return 1;
    }
}

void Plugin::Flags::ShowVariablesMap() {
    try {
        for (const auto& it : _flags) {
            std::cout << it.first.c_str() << " ";
            auto& value = it.second.value();
            if (auto v = boost::any_cast<bool>(&value))
                std::cout << *v << std::endl;
            else if (auto v = boost::any_cast<int>(&value))
                std::cout << *v << std::endl;
            else if (auto v = boost::any_cast<int64_t>(&value))
                std::cout << *v << std::endl;
            else if (auto v = boost::any_cast<std::string>(&value))
                std::cout << *v << std::endl;
            else
                _logger->warn("variable map value type not supported");
        }
    }
    catch (std::exception &e) {
        _logger->error(e.what());
    }
}

bool Plugin::Flags::GetFlagBoolValue(const char *flagKey) {
    try {
        if (_flags.count(flagKey)) {
            auto &value = _flags[flagKey].value();
            if (auto v = boost::any_cast<bool>(&value)) {
                return *v;
            }
        }

        _logger->warn("{0} is not a bool type", flagKey);
        return false;
    }
    catch (std::exception &e) {
        _logger->error(e.what());
        return false;
    }
}

int Plugin::Flags::GetFlagIntValue(const char *flagKey) {
    try {
        if (_flags.count(flagKey)) {
            auto &value = _flags[flagKey].value();
            if (auto v = boost::any_cast<int>(&value)) {
                return *v;
            }
        }
        _logger->warn("{0} is not an int type", flagKey);
        return 0;
    }
    catch (std::exception &e) {
        _logger->error(e.what());
        return 0;
    }
}

int64_t Plugin::Flags::GetFlagInt64Value(const char *flagKey) {
    try {
        if (_flags.count(flagKey)) {
            auto &value = _flags[flagKey].value();
            if (auto v = boost::any_cast<int64_t>(&value)) {
                return *v;
            }
        }
        _logger->warn("{0} is not an int64 type", flagKey);
        return 0;
    }
    catch (std::exception &e) {
        _logger->error(e.what());
        return 0;
    }
}

std::string Plugin::Flags::GetFlagStrValue(const char *flagKey) {
    try {
        if (_flags.count(flagKey)) {
            auto &value = _flags[flagKey].value();
            if (auto v = boost::any_cast<std::string>(&value)) {
                return *v;
            }
        }
        _logger->warn("{0} is not a string type", flagKey);
        return "";
    }
    catch (std::exception &e) {
        _logger->error(e.what());
        return "";
    }
}

void Plugin::Flags::SetFlagsLogLevel(const int &logLevel /*=2*/) {
    try {
        switch (logLevel) {
        case 0/*Panic*/:
            _logger->set_level(spd::level::critical);
            break;
        case 1/*Fatal*/: 
            _logger->set_level(spd::level::critical);
            break;
        case 2/*Error*/:
            _logger->set_level(spd::level::err);
            break;
        case 3/*Warn*/: 
            _logger->set_level(spd::level::warn);
            break;
        case 4/*Info*/:
            _logger->set_level(spd::level::info);
            break;
        case 5/*Debug*/:
            _logger->set_level(spd::level::debug);
            break;
        default:
            _logger->warn("log level {0} not supported", logLevel);
            break;
        }
    }
    catch (std::exception &e) {
        _logger->error(e.what());
    }
}
