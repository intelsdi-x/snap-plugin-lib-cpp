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
#include "snap/plugin.h"
#include "snap/config.h"
#include "snap/metric.h"
#include "../src/snap/lib_setup_impl.h"
#include "gmock/gmock.h"
#include "snap/flags.h"

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "mocks.h"

using std::function;
using std::string;
using std::unique_ptr;
using Plugin::Metric;
using Plugin::ConfigPolicy;
using Plugin::StringRule;
using ::testing::Return;
using ::testing::Invoke;
using ::testing::_;

class MockExporter : public Plugin::PluginExporter {
public:
  MOCK_METHOD2(ExportPlugin, std::future<void>(std::shared_ptr<Plugin::PluginInterface> plugin, const Plugin::Meta* meta));
};

class PluginTest : public ::testing::Test {
protected:
  function<unique_ptr<Plugin::PluginExporter, function<void(Plugin::PluginExporter*)>>()> default_exporter_provider;

  virtual void SetUp() {
    default_exporter_provider = Plugin::LibSetup::exporter_provider;
  }

  virtual void TearDown() {
    Plugin::LibSetup::exporter_provider = default_exporter_provider;
  }
};

TEST_F(PluginTest, MetaCtorWorks) {
  Plugin::Meta meta(Plugin::Processor, "average", 123);
  EXPECT_EQ(123, meta.version);
  EXPECT_EQ("average", meta.name);
  EXPECT_EQ(Plugin::Processor, meta.type);
}

TEST_F(PluginTest, CollectorInterfaceWorks) {
  MockCollector mock;
  EXPECT_EQ(Plugin::Collector, mock.GetType());
  EXPECT_TRUE(mock.IsCollector() != nullptr);
  EXPECT_EQ(nullptr, mock.IsProcessor());
  EXPECT_EQ(nullptr, mock.IsPublisher());
}

TEST_F(PluginTest, ProcessorInterfaceWorks) {
  MockProcessor mock;
  EXPECT_EQ(Plugin::Processor, mock.GetType());
  EXPECT_TRUE(mock.IsProcessor() != nullptr);
  EXPECT_EQ(nullptr, mock.IsCollector());
  EXPECT_EQ(nullptr, mock.IsPublisher());
}

TEST_F(PluginTest, PublisherInterfaceWorks) {
  MockPublisher mock;
  EXPECT_EQ(Plugin::Publisher, mock.GetType());
  EXPECT_TRUE(mock.IsPublisher() != nullptr);
  EXPECT_EQ(nullptr, mock.IsCollector());
  EXPECT_EQ(nullptr, mock.IsProcessor());
}

TEST_F(PluginTest, PluginExceptionCtorWorks) {
  auto exception = Plugin::PluginException("nothing serious really");
  EXPECT_EQ(std::string("nothing serious really"), exception.what());
}

TEST_F(PluginTest, DefaultPluginExporterIsAvailable) {
  Plugin::PluginExporter* exporter = Plugin::LibSetup::exporter_provider().get();
  EXPECT_TRUE(exporter != nullptr);
}

TEST_F(PluginTest, StartCollectorInvokesExporterAndWaits) {
  MockExporter exporter;
  MockCollector collector;
  Plugin::Flags cli;
  Plugin::Meta meta(Plugin::Processor, "average", 123);
  Plugin::PluginInterface* actPlugin;
  const Plugin::Meta* actMeta;
  bool completionCalled = false;
  auto reporter = [&] (std::shared_ptr<Plugin::PluginInterface> plugin, const Plugin::Meta* meta) {
    actPlugin = plugin.get();
    actMeta = meta;
    return std::async(std::launch::deferred, [&](){ completionCalled |= true; });
  };
  ON_CALL(exporter, ExportPlugin(_, _))
    .WillByDefault(Invoke(reporter));
  Plugin::LibSetup::exporter_provider = [&]{
    return unique_ptr<Plugin::PluginExporter, function<void(Plugin::PluginExporter*)>>(&exporter, [](Plugin::PluginExporter*){}); };
  Plugin::start_collector(&collector, meta,cli);

  EXPECT_EQ("average", actMeta->name);
  EXPECT_EQ(actPlugin, &collector);
  EXPECT_EQ(true, completionCalled);
}

TEST_F(PluginTest, StartProcessorInvokesExporterAndWaits) {
  MockExporter exporter;
  MockProcessor processor;
  Plugin::Meta meta(Plugin::Processor, "average", 123);
  Plugin::PluginInterface* actPlugin;
  const Plugin::Meta* actMeta;
  bool completionCalled = false;
  auto reporter = [&] (std::shared_ptr<Plugin::PluginInterface> plugin, const Plugin::Meta* meta) {
    actPlugin = plugin.get();
    actMeta = meta;
    return std::async(std::launch::deferred, [&](){ completionCalled |= true; });
  };
  ON_CALL(exporter, ExportPlugin(_, _))
    .WillByDefault(Invoke(reporter));
  Plugin::LibSetup::exporter_provider = [&]{
    return unique_ptr<Plugin::PluginExporter, function<void(Plugin::PluginExporter*)>>(&exporter, [](Plugin::PluginExporter*){}); };
  Plugin::start_processor(&processor, meta);

  EXPECT_EQ("average", actMeta->name);
  EXPECT_EQ(actPlugin, &processor);
  EXPECT_EQ(true, completionCalled);
}
TEST_F(PluginTest, StartPublisherInvokesExporterAndWaits) {
  MockExporter exporter;
  MockPublisher publisher;
  Plugin::Meta meta(Plugin::Processor, "average", 123);
  Plugin::PluginInterface* actPlugin;
  const Plugin::Meta* actMeta;
  bool completionCalled = false;
  auto reporter = [&] (std::shared_ptr<Plugin::PluginInterface> plugin, const Plugin::Meta* meta) {
    actPlugin = plugin.get();
    actMeta = meta;
    return std::async(std::launch::deferred, [&](){ completionCalled |= true; });
  };
  ON_CALL(exporter, ExportPlugin(_, _))
    .WillByDefault(Invoke(reporter));
  Plugin::LibSetup::exporter_provider = [&]{
    return unique_ptr<Plugin::PluginExporter, function<void(Plugin::PluginExporter*)>>(&exporter, [](Plugin::PluginExporter*){}); };
  Plugin::start_publisher(&publisher, meta);

  EXPECT_EQ("average", actMeta->name);
  EXPECT_EQ(actPlugin, &publisher);
  EXPECT_EQ(true, completionCalled);
}

TEST_F(PluginTest, TestTLSVariables) {
  int argc = 8;
  char* argv[]{(char*)"this", (char*)"--tls", (char*)"--cert-path", (char*)"fake_path", (char*)"--key-path", (char*)"fake_path", (char*)"--root-cert-path", (char*)"fake_path"};
  Plugin::Flags cli(argc, argv);

  Plugin::Meta meta(Plugin::Collector, "average", 123, &cli);

  EXPECT_EQ(meta.tls_enabled, true);

  EXPECT_EQ(meta.tls_certificate_key_path, "fake_path");
  EXPECT_EQ(meta.tls_certificate_crt_path, "fake_path");
  EXPECT_EQ(meta.tls_certificate_authority_paths, "fake_path");
}
