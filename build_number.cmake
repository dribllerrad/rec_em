
set(build_file ${CMAKE_CURRENT_SOURCE_DIR}/BUILD_NUMBER)

if(NOT EXISTS ${build_file})
    file(WRITE ${build_file} 0000)
else()
    file(READ ${build_file} build_number)
    math(EXPR new_build_number "${build_number} + 1")
    file(WRITE ${build_file} ${new_build_number})
endif()
