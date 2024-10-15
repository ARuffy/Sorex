cmake_minimum_required(VERSION 3.12.0)

if(NOT ${SANDBOX_NAME})
    set(SANDBOX_NAME "Sandbox")
endif()

project(${SANDBOX_NAME})

include(BuildTools)
include(TargetPlatform)

set(CMAKE_CXX_STANDARD 20)

add_executable(${SANDBOX_NAME} "${CMAKE_CURRENT_LIST_DIR}/Main.cpp")

sorex_target_compile_definitions(${SANDBOX_NAME})
sorex_target_compile_options(${SANDBOX_NAME})

# get_property(RUFFY_ENGINE_INCLUDE_DIRS TARGET Engine PROPERTY INCLUDE_DIRECTORIES)

# target_include_directories(${SANDBOX_NAME} PRIVATE ${RUFFY_ENGINE_INCLUDE_DIRS})
# target_link_libraries(${SANDBOX_NAME} PUBLIC Engine)

