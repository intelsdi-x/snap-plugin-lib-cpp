/*
http://www.apache.org/licenses/LICENSE-2.0.txt
Copyright 2016 Intel Corporation
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
#include "snap/config.h"
#include "gtest/gtest.h"

#include <sstream>
#include <string>
#include <vector>

using Plugin::Config;
using Plugin::ConfigPolicy;
using Plugin::StringRule;
using rpc::ConfigMap;

TEST(ConfigPolicyTest, AddStringRuleWorks) {
    ConfigPolicy policy;
    policy.add_rule({"baz", "lol"},
                    Plugin::StringRule{
                            "dbname",
                            {
                                    "inventory",
                                    true
                            }
                    });
    EXPECT_EQ(1, policy.string_policy_size());
    EXPECT_EQ(1, policy.string_policy().count("baz.lol"));
    EXPECT_EQ("inventory", policy.mutable_string_policy()->at("baz.lol").rules().at("dbname").default_());
    EXPECT_EQ(true, policy.mutable_string_policy()->at("baz.lol").rules().at("dbname").required());
}

TEST(ConfigPolicyTest, AddIntRuleWorks) {
    ConfigPolicy policy;
    policy.add_rule({"baz", "lol"},
                    Plugin::IntRule{
                            "dbport",
                            {
                                    4505,
                                    true,
                                    1024,
                                    32767
                            }
                    });
    EXPECT_EQ(1, policy.integer_policy_size());
    EXPECT_EQ(1, policy.integer_policy().count("baz.lol"));
    EXPECT_EQ(4505, policy.mutable_integer_policy()->at("baz.lol").rules().at("dbport").default_());
    EXPECT_EQ(1024, policy.mutable_integer_policy()->at("baz.lol").rules().at("dbport").minimum());
    EXPECT_EQ(32767, policy.mutable_integer_policy()->at("baz.lol").rules().at("dbport").maximum());
    EXPECT_EQ(true, policy.mutable_integer_policy()->at("baz.lol").rules().at("dbport").required());
}

TEST(ConfigPolicyTest, AddBoolRuleWorks) {
    ConfigPolicy policy;
    policy.add_rule({"baz", "lol"},
                    Plugin::BoolRule{
                            "transactional",
                            {
                                    false,
                                    true
                            }
                    });
    EXPECT_EQ(1, policy.bool_policy_size());
    EXPECT_EQ(1, policy.bool_policy().count("baz.lol"));
    EXPECT_EQ(false, policy.mutable_bool_policy()->at("baz.lol").rules().at("transactional").default_());
    EXPECT_EQ(true, policy.mutable_bool_policy()->at("baz.lol").rules().at("transactional").required());
}

TEST(PluginConfigTest, GetBoolValueWorks) {
    rpc::ConfigMap baseMap;
    baseMap.mutable_boolmap()->insert(google::protobuf::Map<std::string, bool>::value_type("her", true));
    Plugin::Config config(baseMap);

    EXPECT_EQ(true, config.get_bool("her"));
}

TEST(PluginConfigTest, GetStringValueWorks) {
    rpc::ConfigMap baseMap;
    baseMap.mutable_stringmap()->insert(google::protobuf::Map<std::string, std::string>::value_type("his", "bonk"));
    Plugin::Config config(baseMap);

    EXPECT_EQ("bonk", config.get_string("his"));
}

TEST(PluginConfigTest, GetIntValueWorks) {
    rpc::ConfigMap baseMap;
    baseMap.mutable_intmap()->insert(google::protobuf::Map<std::string, google::protobuf::int64>::value_type("its", 0xface));
    Plugin::Config config(baseMap);

    EXPECT_EQ(0xface, config.get_int("its"));
}
