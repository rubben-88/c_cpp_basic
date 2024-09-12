# Minimum CMake version
cmake_minimum_required(VERSION 3.28)

# Project name and version
project(project01 VERSION 1.0)

# Build Type
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -g")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -O3")
endif()

# C++ and C standards
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

# Minimum CGG version
find_program(GCC_VERSION gcc)
if(NOT GCC_VERSION)
    message(FATAL_ERROR "GCC not found.")
endif()
execute_process(COMMAND ${GCC_VERSION} --version OUTPUT_VARIABLE GCC_VERSION_OUTPUT)
string(REGEX MATCH "([0-9]+)\\.([0-9]+)" GCC_VERSION_MATCH ${GCC_VERSION_OUTPUT})
if(${GCC_VERSION_MATCH} VERSION_LESS "13.2")
    message(FATAL_ERROR "GCC version 13.2 or higher is required.")
endif()

# Minimum G++ version
find_program(GPP_VERSION g++)
if(NOT GPP_VERSION)
    message(FATAL_ERROR "G++ not found.")
endif()
execute_process(COMMAND ${GPP_VERSION} --version OUTPUT_VARIABLE GPP_VERSION_OUTPUT)
string(REGEX MATCH "([0-9]+)\\.([0-9]+)" GPP_VERSION_MATCH ${GPP_VERSION_OUTPUT})
if(${GPP_VERSION_MATCH} VERSION_LESS "13.2")
    message(FATAL_ERROR "G++ version 13.2 or higher is required.")
endif()

# Header files
include_directories(include)
file(GLOB_RECURSE INCLUDES "include/*")

# Source files
include_directories(src)
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c")
add_executable("${PROJECT_NAME}" ${SOURCES})

#= External Libraries =======================================================================#
set(LIBS_DIR "${CMAKE_SOURCE_DIR}/libs")

function(add_library_from_dir libname)
    set(LIB_DIR "${LIBS_DIR}/${libname}")
    if (EXISTS "${LIB_DIR}")
        message(STATUS "Found library: ${libname}")
        include_directories("${LIB_DIR}/include")
        file(GLOB_RECURSE LIB_SOURCES "${LIB_DIR}/lib/*.a")
        if (LIB_SOURCES)
            target_link_libraries("${PROJECT_NAME}" ${LIB_SOURCES})
        endif()
    else()
        message(WARNING "Library directory ${LIB_DIR} not found.")
    endif()
endfunction()

file(GLOB directories LIST_DIRECTORIES true "${CMAKE_SOURCE_DIR}/libs/*")
foreach(dir ${directories})
    if(IS_DIRECTORY ${dir})
        get_filename_component(dir_name ${dir} NAME)
        add_library_from_dir("${dir_name}")
    endif()
endforeach()
#============================================================================================#
