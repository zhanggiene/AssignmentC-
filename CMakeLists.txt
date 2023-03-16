cmake_minimum_required(VERSION 3.24)
project(crypto)

set(CMAKE_CXX_STANDARD 17)
set(Boost_USE_STATIC_LIBS ON)
# Find Boost libraries
find_package(Boost REQUIRED COMPONENTS system)

# Add Boost include directories
include_directories(${Boost_INCLUDE_DIRS})

# Add FlatBuffers directly to our build. This defines the `flatbuffers` target.
set(FLATBUFFERS_SRC_DIR ./flatbuffers)
add_subdirectory(${FLATBUFFERS_SRC_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}/flatbuffers-build
        EXCLUDE_FROM_ALL)

# Now simply link against flatbuffers as needed to your already declared target.
# The flatbuffers target carry header search path automatically if CMake > 2.8.11.

add_executable(crypto main.cpp)
add_executable(sender sender.cpp)
target_link_libraries (crypto PRIVATE flatbuffers ${Boost_LIBRARIES})
target_link_libraries (sender PRIVATE flatbuffers ${Boost_LIBRARIES})