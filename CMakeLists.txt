cmake_minimum_required(VERSION 3.12.0)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake/")

# ##############################################################################
# Project
# ##############################################################################
set(PROJECT_NAME "Sorex")
project(
  ${PROJECT_NAME}
  VERSION 0.1.0
  DESCRIPTION "Simple OpenGL Rendering Engine eXtended"
  # [HOMEPAGE_URL <url-string>]
  LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CONFIGURATION_TYPES
    "Debug;Release"
    CACHE STRING "Configuration types" FORCE
)

option(SOREX_BUILD_SANDBOX "Build sandbox application" ON)
option(SOREX_BUILD_TESTS "Build tests." OFF)

set(SOREX_ENGINE_DIR "${CMAKE_CURRENT_LIST_DIR}/Engine")
add_subdirectory(${SOREX_ENGINE_DIR})

# ##############################################################################
# Sandbox
# ##############################################################################
if(SOREX_BUILD_SANDBOX)
  set(SANDBOX_NAME "Sandbox")
  add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/${SANDBOX_NAME}")

  if(MSVC)
    set_property(
      DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT
                                                     ${SANDBOX_NAME}
    )
  endif()
endif()

# ##############################################################################
# Tests
# ##############################################################################
if(SOREX_BUILD_TESTS)
  message(STATUS "[${PROJECT_NAME}] Enable testing")

  include(CTest)
  enable_testing()

  add_subdirectory("${CMAKE_SOURCE_DIR}/Engine/Tests")
endif()
