cmake_minimum_required(VERSION 3.0.0)

project(rec_em LANGUAGES CXX)

set(PROJECT_VERSION_MAJOR 1)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_PATCH 0)



####################################################################
# Versioning


if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/.git)
    find_package(Git)
    if(GIT_FOUND)
        execute_process(
            COMMAND git log -1 --format=%h
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_COMMIT
            ERROR_VARIABLE GIT_COMMIT_ERROR
            OUTPUT_STRIP_TRAILING_WHITESPACE
            #ERROR_QUIET
        )
    if(GIT_COMMIT_ERROR)
        set(GIT_COMMIT 0)
    endif(GIT_COMMIT_ERROR)
    message( STATUS "Git commit: ${GIT_COMMIT}" )
    else(GIT_FOUND)
        set(GIT_COMMIT 0)
    endif(GIT_FOUND)
endif(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/.git)

set(generated_header_dir ${CMAKE_BINARY_DIR}/generated)


####################################################################
# If rec_em is a submodule in another project it is aliased as an
# interface library and exposed to the main project's scope. This
# allows the generated header to be included in the main project's
# source files.
get_directory_property(hasParent PARENT_DIRECTORY)
if(hasParent)
  message(STATUS "\n\n
          rec_em Message: Has a parent scope.\n
          Setting rec_em build type to Release and exposing\n
          header directory to parent project.\n\n")
  set(CMAKE_BUILD_TYPE Release)
  set(generated_header_dir ${CMAKE_BINARY_DIR}/generated PARENT_SCOPE)
  add_library(rec_em_interface INTERFACE)
  add_library(rec_em::rec_em ALIAS rec_em_interface)
  target_include_directories(rec_em_interface INTERFACE ${generated_header_dir})
endif()

set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/BUILD_NUMBER)
set(build_file ${CMAKE_CURRENT_SOURCE_DIR}/BUILD_NUMBER)
file(STRINGS ${build_file} project_build_number LIMIT_COUNT 1)
set(PROJECT_VERSION_TWEAK ${project_build_number}) # CACHE STRING "Variable to set project build number")

set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}.${PROJECT_VERSION_TWEAK}")

set(CAPITOLIZED_PROJECT_NAME REC_EM)

set(VERSION_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}_version.in)
set(VERSION_HEADER ${generated_header_dir}/${PROJECT_NAME}_version.hpp)

configure_file(
    ${VERSION_INPUT}
    ${VERSION_HEADER}

)
####################################################################


####################################################################
# Build executable
add_executable(${PROJECT_NAME} src/${PROJECT_NAME}.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${generated_header_dir} include)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)


#[=======================================================================[
Note about compiler support
********************************************************************
Despite supporting c++17, some compilers still need the extra
filesystem library linked in (-lstdc++fs).  Specifically, this applies
to gcc 8.x.  Gcc =< 7.x does not support c++17 and gcc >= 9.x has the
filesystem library included in the standard.
This fix is not necessary in Clang >= 8.0 and MSVC >= 2017.
********************************************************************
#]=======================================================================]


if(${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
    message("Found Clang Compiler ...")
    if(${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 8)
        message(
            FATAL_ERROR
            "\n\nClang is out of date...\n
            Update Clang to version 8.0 or greater")
    endif()
endif()

if(${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
    message("Found GNU Compiler (gcc)...")
    if(${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 9)
        message("\n\nGNU is old!!!\n\n")
        message("\nAdding filesystem library\n")
        target_link_libraries(${PROJECT_NAME} stdc++fs)
    endif()
endif()

#TODO Add check for MSVC compiler.

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
####################################################################


####################################################################
# Post Build: incriment the build number
add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/build_number.cmake
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
####################################################################

####################################################################
## For Debugging System Variables
# cmake_host_system_information(RESULT os_name QUERY OS_NAME)
# message("OS name detected: ${os_name}")
# if(${os_name} STREQUAL Linux)

#     message("\n\n...if works...\n\n")
#     message("Cmake host system name: ${CMAKE_HOST_SYSTEM_NAME}")
#     message("Compiler ID detected: ${CMAKE_CXX_COMPILER_ID}")
#     message("Compiler version: ${CMAKE_CXX_COMPILER_VERSION}")
#     message("CXX_STANDARD: ${CXX_STANDARD}\n\n")

# endif()
