cmake_minimum_required(VERSION 4.4)

project(maven_c_app LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# 让单配置和多配置生成器都把可执行文件放到同一个目录。
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
foreach(CONFIG DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
    set("CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG}" "${CMAKE_BINARY_DIR}/bin")
endforeach()

# 新增 .c/.h 后由 CMake 自动重新扫描，无需逐个维护文件列表。
file(GLOB_RECURSE PROJECT_SOURCES
        CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/main/c/*.c"
)

file(GLOB_RECURSE PROJECT_HEADERS
        CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/main/include/*.h"
)

# 把 main.c 与可复用业务源码分开，主程序和单元测试共享同一个库。
set(PROJECT_MAIN_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/main/c/main.c")
list(REMOVE_ITEM PROJECT_SOURCES "${PROJECT_MAIN_SOURCE}")

add_library(project_lib STATIC
        ${PROJECT_SOURCES}
        ${PROJECT_HEADERS}
)

target_include_directories(project_lib
        PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/main/include"
)

add_executable(project_app "${PROJECT_MAIN_SOURCE}")
target_link_libraries(project_app PRIVATE project_lib)
set_target_properties(project_app PROPERTIES OUTPUT_NAME "${artifactId}")

file(GLOB_RECURSE PROJECT_TEST_SOURCES
        CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/test/c/*.c"
)

enable_testing()
add_executable(project_tests ${PROJECT_TEST_SOURCES})
target_link_libraries(project_tests PRIVATE project_lib)
add_test(NAME unit_tests COMMAND project_tests)
