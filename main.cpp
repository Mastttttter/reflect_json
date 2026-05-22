#include <algorithm>
#include <atomic>
#include <chrono>
#include <concepts>
#include <fstream>
#include <functional>
#include <future>
#include <iostream>
#include <numeric>
#include <print>
#include <queue>
#include <ranges>
#include <thread>
#include <utility>
#include <vector>
#include "json_helper.hpp"

struct[[= json_helper::json_meta::serializable]] Server_ {
  int port = 8080;
};

struct[[= json_helper::json_meta::serializable]] Logging_ {
  std::string log_file_path = "logs/server.log";
  [[= json_helper::json_meta::default_string("asdf")]] std::string log_level =
      "info";
  size_t log_file_size = 10 * 1024 * 1024uz;
  int lof_file_count = 5;
  bool console_output = false;
};

enum class test {
  red,
  blue,
  green,
};

struct[[= json_helper::json_meta::serializable]] Config_ {
  [[= json_helper::json_meta::default_value{
      Server_{.port = 7890}}]] Server_ server;
  Logging_ logging;
  [[= json_helper::json_meta::default_value{test::green}]] test test_;
  [[= json_helper::json_meta::ignore]] test test1;
};

int main() {
  std::stringstream ss{R"({})"};
  auto data = nlohmann::json::parse(ss);
  auto cfg = json_helper::from_json_reflect<Config_>(data);
  auto ndata = json_helper::reflect_to_json(cfg);
  std::println("{}", ndata.dump(4));
  return 0;
}
