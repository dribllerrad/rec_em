cmake_minimum_required(VERSION 3.0.0)

project(rec_em)

set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 1)
set(PROJECT_VERSION_PATCH 0)



####################################################################
# Versioning
# set(MAJOR_VERSION ${PROJECT_VERSION_MAJOR})
# set(MINOR_VERSION ${PROJECT_VERSION_MINOR})

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

get_directory_property(hasParent PARENT_DIRECTORY)
if(hasParent)
  message(STATUS "\nHas a parent scope.
  Setting rec_em build type to Release and exposing
  header directory to parent project.\n")
  set(CMAKE_BUILD_TYPE Release)
  set(generated_header_dir ${CMAKE_BINARY_DIR}/generated PARENT_SCOPE)
endif()

set(build_file ${CMAKE_CURRENT_SOURCE_DIR}/BUILD_NUMBER)
file(READ ${build_file} project_build_number)
set(PROJECT_VERSION_TWEAK ${project_build_number}) # CACHE STRING "Variable to set project build number")

set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}.${PROJECT_VERSION_TWEAK}")

set(CAPITOLIZED_PROJECT_NAME REC_EM)

set(VERSION_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}_version.in)
set(VERSION_HEADER ${generated_header_dir}/${PROJECT_NAME}_version.h)

configure_file(
    ${VERSION_INPUT}
    ${VERSION_HEADER}
  #${CMAKE_CURRENT_SOURCE_DIR}/include/version.in
  #${CMAKE_BINARY_DIR}/generated/version.h
)
####################################################################


####################################################################
# Build executable
add_executable(${PROJECT_NAME} src/${PROJECT_NAME}.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${generated_header_dir})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
#target_link_libraries(${PROJECT_NAME} stdc++fs)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
    message("Found Clang!!!")
endif()

if(${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
    message("Found GNU!!!")
    if(${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 9)
        message("\n\nGNU is old!!!\n\n")
        message("\nAdding filesystem library\n")
        target_link_libraries(${PROJECT_NAME} stdc++fs)
    endif()
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#if(LINUX)
#    message("\nAdding filesystem library\n")
#    target_link_libraries(${PROJECT_NAME} stdc++fs)
#endif(LINUX)
####################################################################


####################################################################
# Post Build: incriment the build number
add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/build_number.cmake
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
####################################################################


cmake_host_system_information(RESULT os_name QUERY OS_NAME)
message("OS name detected: ${os_name}")
if(${os_name} STREQUAL Linux)

    message("\n\n...if works...\n\n")
    message("Cmake host system name: ${CMAKE_HOST_SYSTEM_NAME}")
    message("Compiler ID detected: ${CMAKE_CXX_COMPILER_ID}")
    message("Compiler version: ${CMAKE_CXX_COMPILER_VERSION}")
    message("CXX_STANDARD: ${CXX_STANDARD}\n\n")

endif()
