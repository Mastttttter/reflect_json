cmake_minimum_required(VERSION 4.3.2)

project(reflect_json LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
find_package(nlohmann_json REQUIRED)

add_executable(reflect_json main.cpp
json_helper.hpp
enum_helper.hpp
)

target_link_libraries(reflect_json nlohmann_json::nlohmann_json)

target_compile_options(reflect_json PRIVATE -freflection)
