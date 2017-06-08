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
#pragma once

#include <boost/program_options.hpp>
#include <iostream>
#include <fstream>
#include <iterator>
#include <spdlog/spdlog.h>

#define COMMAND_DESC "COMMANDS"
#define GLOBAL_DESC "GLOBAL OPTIONS"
#define ADDITIONAL_DESC ""

namespace po = boost::program_options;
namespace spd = spdlog;

using namespace std;

namespace Plugin {
    //helper function to simplify main part
    template<class T>
    ostream& operator<<(ostream& os, const vector<T>& v) {
        copy(v.begin(), v.end(), ostream_iterator<T>(os, " "));
        return os;
    };
    
    class Flags {
    private:
        std::shared_ptr<spd::logger> _logger;

        po::variables_map _flags;
        
        // Initial option descriptions
        po::options_description _command{COMMAND_DESC}, _global{GLOBAL_DESC};
        po::options_description _hidden, _additional{ADDITIONAL_DESC};
        
        // Combined option descriptions
        po::options_description _visible, _command_line, _config_file;

        // Default variables
        int _log_level, _stand_alone_port, _max_collect_duration;
        std::string _options_file, _listen_port, _listen_addr, _cert_path, _key_path, _root_cert_paths;
        int64_t _max_metrics_buffer;

    public:
        enum FlagType {
            Bool,
            Int,
            String
        };

        enum FlagLevel {
            Command,
            Global,
            Hidden,
            Custom
        };

        Flags() {
            _logger = spdlog::stdout_logger_mt("flags");
        }

        Flags(const int &argc, char **argv) {
            _logger = spdlog::stdout_logger_mt("flags");
            this->SetFlags();
            this->ParseFlags(argc, argv);
        }

        int addDefaultCommandFlags();
        int addDefaultGlobalFlags();
        int addDefaultHiddenFlags();
        int SetDefaultFlags();

        int addBoolFlag(const char *optionName, const char *description,
                        FlagLevel flagLevel);
        int addIntFlag(const char *optionName, const char *description,
                        FlagLevel flagLevel);
        int addStringFlag(const char *optionName, const char *description,
                        FlagLevel flagLevel);
        int AddFlag(const char *optionName, const char *description, 
                        FlagType flagType, FlagLevel flagLevel);

        int setVisibleFlags();
        int setCommandLineFlags();
        int setConfigFileFlags();
        int SetCombinedFlags();

        int SetFlags() { 
            if (SetDefaultFlags() != 0) return 1;
            if (SetCombinedFlags() != 0) return 1;
            return 0;
        }

        int parseCommandLineFlags(const int &argc, char **argv);
        int parseConfigFileFlags(std::string filePathAndName = "");
        int ParseFlags(const int &argc, char **argv, std::string filePathAndName = "");

        void ShowVariablesMap();
        bool IsParsedFlag(const char *flagKey) { return _flags.count(flagKey); }
        po::variables_map GetFlagsVM() { return _flags; }

        bool GetFlagBoolValue(const char *flagKey);
        int GetFlagIntValue(const char *flagKey);
        int64_t GetFlagInt64Value(const char *flagKey);
        std::string GetFlagStrValue(const char *flagKey);

        void SetFlagsLogLevel(const int &logLevel = 2);

        int helpFlagCalled() {
            std::cout << _visible << std::endl;
            return 0;
        }

        po::options_description GetCommandOptions() { return _command; }
        void PrintCommandOptions() { std::cout << _command << std::endl; }

        po::options_description GetGlobalOptions() { return _global; }
        void PrintGlobalOptions() { std::cout << _global << std::endl; }

        po::options_description GetHiddenOptions() { return _hidden; }
        void PrintHiddenOptions() { std::cout << _hidden << std::endl; }

        po::options_description GetAdditionalOptions() { return _additional; }
        void PrintAdditionalOptions() { std::cout << _additional << std::endl; }

        po::options_description GetVisibleOptions() { return _visible; }
        void PrintVisibleOptions() { std::cout << _visible << std::endl; }

        po::options_description GetCommandLineOptions() { return _command_line; }
        void PrintCommandLineOptions() { std::cout << _command_line << std::endl; }

        po::options_description GetConfigFileOptions() { return _config_file; }
        void PrintConfigFileOptions() { std::cout << _config_file << std::endl; }
    };
} // namespace Plugin
