cmake_minimum_required(VERSION 3.12.0)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake/")

# ###############################
# Project
# ###############################
set(PROJECT_NAME "Sorex")
project(${PROJECT_NAME}
     VERSION 0.1.0
     DESCRIPTION "Simple OpenGL Rendering Engine eXtended"
     # [HOMEPAGE_URL <url-string>]
     LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Configuration types" FORCE)
if(TRUE)
  set(SOREX_DEBUG TRUE)
  message(STATUS "[${PROJECT_NAME}] Configuration mode: Debug")
endif()

# ###############################
# Sandbox
# ###############################
if(SOREX_DEBUG)
  set(SANDBOX_NAME "Sandbox")
  add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/${SANDBOX_NAME}")

  if(MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${SANDBOX_NAME})
  endif()
endif()

