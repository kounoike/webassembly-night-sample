cmake_minimum_required(VERSION 2.8)
project(cppmain LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s ALLOW_MEMORY_GROWTH=1 -s USE_ZLIB=1 -s ASSERTIONS=1 -s DISABLE_EXCEPTION_CATCHING=0 --bind -O3 -s LLD_REPORT_UNDEFINED -s DEMANGLE_SUPPORT=1")
set(BUILD_WASM "build_wasm_simd")
# set(BUILD_WASM "build_wasm")

include_directories(/build/opencv/${BUILD_WASM}/install/include/opencv4)

file(GLOB_RECURSE sources1 "src/main.cpp")
add_executable(${PROJECT_NAME} ${sources1})

file(GLOB opencv_core "/build/opencv/${BUILD_WASM}/install/lib/*.a")
file(GLOB opencv_3rdparty "/build/opencv/${BUILD_WASM}/install/lib/opencv4/3rdparty/*.a")
target_link_libraries(${PROJECT_NAME} ${opencv_core} ${opencv_3rdparty})
