cmake_minimum_required(VERSION 3.4...3.18)
project(gloss)

add_subdirectory(pybind11)
pybind11_add_module(gloss ./src/bindings.cpp)

add_library(worker1 STATIC ./src/utils.cpp)
set_target_properties(worker1 PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Add include directory (assumes /include/ is at the root of your repo)
target_include_directories(gloss PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(worker1 PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Define version info passed from setup.py
target_compile_definitions(gloss PRIVATE VERSION_INFO=${GLOSS_VERSION_INFO})

# Link the worker1 static library into the gloss module
target_link_libraries(gloss PRIVATE worker1)
