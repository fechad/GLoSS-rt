cmake_minimum_required(VERSION 3.4...3.18)
project(gloss)

# Help CMake find GDAL and other libraries in conda environment
if(DEFINED ENV{CONDA_PREFIX})
    message(STATUS "Using Conda environment: $ENV{CONDA_PREFIX}")
    set(CMAKE_PREFIX_PATH "$ENV{CONDA_PREFIX}" ${CMAKE_PREFIX_PATH})
    include_directories("$ENV{CONDA_PREFIX}/include")
    link_directories("$ENV{CONDA_PREFIX}/lib")
    
    # Set PROJ_LIB for GDAL
    set(ENV{PROJ_LIB} "$ENV{CONDA_PREFIX}/share/proj")
endif()

find_package(GDAL REQUIRED)
find_package(fmt REQUIRED)

# Use pybind11 subdirectory
add_subdirectory(pybind11)

pybind11_add_module(gloss ./src/bindings.cpp)

add_library(worker1 STATIC ./src/utils.cpp)
set_target_properties(worker1 PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Add include directory (assumes /include/ is at the root of your repo)
target_include_directories(gloss PRIVATE 
    ${CMAKE_SOURCE_DIR}/include 
    ${GDAL_INCLUDE_DIRS}
)
target_include_directories(worker1 PRIVATE 
    ${CMAKE_SOURCE_DIR}/include 
    ${GDAL_INCLUDE_DIRS}
)

# Define version info passed from setup.py
target_compile_definitions(gloss PRIVATE VERSION_INFO=${GLOSS_VERSION_INFO})

# Link the worker1 static library into the gloss module
target_link_libraries(gloss PRIVATE worker1 ${GDAL_LIBRARIES} fmt::fmt)
target_link_libraries(worker1 PRIVATE ${GDAL_LIBRARIES})

# Optional: Create standalone executable
add_executable(gloss_standalone ./src/gloss.cpp ./src/utils.cpp)
target_include_directories(gloss_standalone PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
    ${GDAL_INCLUDE_DIRS}
)
target_link_libraries(gloss_standalone PRIVATE
    ${GDAL_LIBRARIES}
    fmt::fmt
)