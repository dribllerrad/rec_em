cmake_minimum_required(VERSION 3.5.0 FATAL_ERROR)

project(rec_em LANGUAGES CXX)

set(PROJECT_VERSION_MAJOR 1)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_PATCH 1)

####################################################################
# Setup
include(GNUInstallDirs)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

if(MSVC)
    set(CMAKE_DEBUG_POSTFIX "d")
endif()

include(AddInstallRPATHSupport)
add_install_rpath_support(BIN_DIRS "${CMAKE_INSTALL_FULL_BINDIR}"
                          LIB_DIRS "${CMAKE_INSTALL_FULL_LIBDIR}"
                          INSTALL_NAME_DIR "${CMAKE_INSTALL_FULL_LIBDIR}"
                          USE_LINK_PATH)



#add_subdirectory(src)
set(EXE_TARGET_NAME ${PROJECT_NAME})
set(${EXE_TARGET_NAME}_SRC src/${EXE_TARGET_NAME}.cpp)
set(generated_header_dir ${CMAKE_BINARY_DIR}/generated)
set(include_dir include)

add_executable(${EXE_TARGET_NAME} ${${EXE_TARGET_NAME}_SRC})
target_include_directories(${PROJECT_NAME} PRIVATE ${generated_header_dir} ${include_dir})
target_compile_features(${EXE_TARGET_NAME} PRIVATE cxx_std_17)

install(TARGETS ${EXE_TARGET_NAME}
        EXPORT  ${PROJECT_NAME}
        DESTINATION bin)

message(STATUS
        "\n\n
        CMAKE_BINARY_DIR = ${CMAKE_BINARY_DIR}
        CMAKE_INSTALL_BINDIR = ${CMAKE_INSTALL_BINDIR}
        CMAKE_RUNTIME_OUTPUT_DIRECTORY = ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        \n\n")


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





####################################################################
# If rec_em is a submodule in another project it is aliased as an
# interface library and exposed to the main project's scope. This
# allows the generated header to be included in the main project's
# source files.

#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# commented for cmake_testing
#get_directory_property(hasParent PARENT_DIRECTORY)
#if(hasParent)
#  message(STATUS "\n\n
#          rec_em Message: Has a parent scope.\n
#          Setting rec_em build type to Release and exposing\n
#          header directory to parent project.\n\n")
#  set(CMAKE_BUILD_TYPE Release)
#  set(generated_header_dir ${CMAKE_BINARY_DIR}/generated PARENT_SCOPE)
#  add_library(rec_em_interface INTERFACE)
#  add_library(rec_em::rec_em ALIAS rec_em_interface)
#  target_include_directories(rec_em_interface INTERFACE ${generated_header_dir})
#endif()

set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/BUILD_NUMBER)
set(build_file ${CMAKE_CURRENT_SOURCE_DIR}/BUILD_NUMBER)
file(STRINGS ${build_file} project_build_number LIMIT_COUNT 1)
set(PROJECT_VERSION_TWEAK ${project_build_number})

set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}.${PROJECT_VERSION_TWEAK}")

set(CAPITOLIZED_PROJECT_NAME REC_EM)

set(VERSION_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}_version.in)
set(VERSION_HEADER ${generated_header_dir}/${PROJECT_NAME}_version.hpp)

configure_file(
    ${VERSION_INPUT}
    ${VERSION_HEADER}

)
####################################################################


include(InstallBasicPackageFiles)
install_basic_package_files(${PROJECT_NAME}
                            VERSION ${PROJECT_VERSION}
                            COMPATIBILITY AnyNewerVersion
                            VARS_PREFIX ${PROJECT_NAME}
                            NO_CHECK_REQUIRED_COMPONENTS_MACRO)


include(AddUninstallTarget)


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
