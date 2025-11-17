cmake_minimum_required(VERSION 3.16)
project(cpp_search_engine VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Use GCC/Clang warning flags by default; MSVC doesn't recognize -Wextra/-Wpedantic
if(MSVC)
	add_compile_options(/W4)
else()
	add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(Threads REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/include)
file(GLOB_RECURSE SRC_FILES src/*.cpp)
add_executable(search_engine ${SRC_FILES})

target_link_libraries(search_engine PRIVATE Threads::Threads)
