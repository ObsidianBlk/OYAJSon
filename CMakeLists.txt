cmake_minimum_required(VERSION 2.8)
project(OYAJSon)

# This project requires the c++11 standard.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Looking for required libraries
#include(FindPkgConfig)

# Set the include and link directories for all required libraries and source code.
include_directories(${OYAJSon_SOURCE_DIR} ${OYAJSon_SOURCE_DIR}/Test)
link_directories(${OYAJSon_SOURCE_DIR})

# I believe this will call the correct shell script.
# NOTE: Will update this later to handle Windows batch scripts.
add_custom_command(
    OUTPUT ${OYAJSon_SOURCE_DIR}/OYAJSon_version.cpp
    COMMAND ${OYAJSon_SOURCE_DIR}/scripts/version.sh ${OYAJSon_SOURCE_DIR}
)

set(core_source_files
    main.cpp
    OYAJSon.h
    OYAJSon.cpp
    OYAJSon_version.cpp
)

add_library(OYAJSon SHARED OYAJSon.h OYAJSon.cpp OYAJSon_version.cpp)
add_executable(OYAJSon_Test Test/test.cpp)
target_link_libraries(OYAJSon_Test LINK_PUBLIC OYAJSon)
install(TARGETS OYAJSon_Test DESTINATION bin)
