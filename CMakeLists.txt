cmake_minimum_required(VERSION 3.20)
project(UltimateLighthouseBeacon VERSION 3.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(CURL REQUIRED libcurl)

# Simple beacon (Linux networking + libcurl)
add_executable(ultimate_beacon ultimate_beacon.cpp)
target_link_libraries(ultimate_beacon ${CURL_LIBRARIES})
target_include_directories(ultimate_beacon PRIVATE ${CURL_INCLUDE_DIRS})

# Simple listener (pure Linux networking)
add_executable(ultimate_listener ultimate_listener.cpp)

# Output to bin directory
set_target_properties(ultimate_beacon ultimate_listener
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
