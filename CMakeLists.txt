cmake_minimum_required(VERSION 3.16)
project(FlowForge VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(FLOWFORGE_BUILD_TESTS "Build FlowForge tests" ON)
option(FLOWFORGE_BUILD_BENCH "Build FlowForge benchmarks" ON)
option(FLOWFORGE_ENABLE_TRACY "Enable Tracy profiling hooks" OFF)

if(MSVC)
  add_compile_options(/W4 /permissive-)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(SDL2 REQUIRED)

add_library(flowforge_core
  src/sim/AStar.cpp
  src/sim/AgentSoA.cpp
  src/sim/FlowField.cpp
  src/sim/Grid.cpp
  src/sim/Simulation.cpp
  src/sim/SpatialGrid.cpp
  src/sim/Metrics.cpp
)

target_include_directories(flowforge_core PUBLIC include)

target_compile_definitions(flowforge_core PUBLIC
  $<$<BOOL:${FLOWFORGE_ENABLE_TRACY}>:FLOWFORGE_TRACY=1>
)

add_executable(flowforge
  src/main.cpp
  src/render/BitmapFont.cpp
  src/render/Renderer.cpp
  src/ui/InputController.cpp
)

target_link_libraries(flowforge PRIVATE flowforge_core SDL2::SDL2)

if(FLOWFORGE_BUILD_TESTS)
  enable_testing()
  add_executable(flowforge_tests
    tests/test_main.cpp
  )
  target_link_libraries(flowforge_tests PRIVATE flowforge_core)
  add_test(NAME flowforge_tests COMMAND flowforge_tests)
endif()

if(FLOWFORGE_BUILD_BENCH)
  add_executable(flowforge_bench bench/bench_main.cpp)
  target_link_libraries(flowforge_bench PRIVATE flowforge_core)
endif()
