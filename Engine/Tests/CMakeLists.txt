cmake_minimum_required(VERSION 3.8.0)

include(CTest)
include(TargetPlatform)

set(ENGINE_UNITTEST_TARGET "SorexEngineUnitTests")
set(SOREX_ENGINE_DIR "${CMAKE_SOURCE_DIR}/Engine")

file(GLOB_RECURSE UNITTEST_SOURCE "${CMAKE_CURRENT_LIST_DIR}/Units/*.cpp")

list(LENGTH UNITTEST_SOURCE unit_number)
message(STATUS "[${ENGINE_UNITTEST_TARGET}] Add ${unit_number} unit(s)")

add_executable(${ENGINE_UNITTEST_TARGET} ${UNITTEST_SOURCE})
target_compile_definitions(${ENGINE_UNITTEST_TARGET}
                           PRIVATE ${TARGET_PLATFORM}=1)

find_package(GTest QUIET)

if(GTEST_FOUND)
  message(STATUS "GoogleTest library -- found")
  set(GTEST_LIBRARIES GTest::GTest GTest::Main)
else()
  message(STATUS "GoogleTest library -- not found")

  set(GTEST_NAME "googletest")
  set(GTEST_TAG v1.15.2)

  include(ExternalProject)
  ExternalProject_Add(
    ${GTEST_NAME}
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG ${GTEST_TAG}
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/${GTEST_NAME}
    CMAKE_ARGS -DBUILD_GMOCK=OFF -Dgtest_force_shared_crt=ON
    INSTALL_COMMAND ""
    TEST_COMMAND "")

  ExternalProject_Get_Property(${GTEST_NAME} SOURCE_DIR BINARY_DIR)
  set(GTEST_INCLUDE_DIR "${SOURCE_DIR}/googletest/include")

  if(MSVC)
    set(GTEST_IMPORTED_LOCATION
        IMPORTED_LOCATION_DEBUG
        "${BINARY_DIR}/lib/Debug/${CMAKE_STATIC_LIBRARY_PREFIX}gtest${CMAKE_STATIC_LIBRARY_SUFFIX}"
        IMPORTED_LOCATION_RELEASE
        "${BINARY_DIR}/lib/Release/${CMAKE_STATIC_LIBRARY_PREFIX}gtest${CMAKE_STATIC_LIBRARY_SUFFIX}"
    )
    set(GTESTMAIN_IMPORTED_LOCATION
        IMPORTED_LOCATION_DEBUG
        "${BINARY_DIR}/lib/Debug/${CMAKE_STATIC_LIBRARY_PREFIX}gtest_main${CMAKE_STATIC_LIBRARY_SUFFIX}"
        IMPORTED_LOCATION_RELEASE
        "${BINARY_DIR}/lib/Release/${CMAKE_STATIC_LIBRARY_PREFIX}gtest_main${CMAKE_STATIC_LIBRARY_SUFFIX}"
    )
  else()
    set(GTEST_IMPORTED_LOCATION
        IMPORTED_LOCATION
        "${BINARY_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}gtest${CMAKE_STATIC_LIBRARY_SUFFIX}"
    )
    set(GTESTMAIN_IMPORTED_LOCATION
        IMPORTED_LOCATION
        "${BINARY_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}gtest_main${CMAKE_STATIC_LIBRARY_SUFFIX}"
    )
  endif()

  # define imported library `libgtest`
  add_library(libgtest IMPORTED STATIC GLOBAL)
  add_dependencies(libgtest ${GTEST_NAME})

  file(MAKE_DIRECTORY ${GTEST_INCLUDE_DIR})
  set_target_properties(
    libgtest
    PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${GTEST_INCLUDE_DIR}"
               INTERFACE_LINK_LIBRARIES "${CMAKE_THREAD_LIBS_INIT}"
               ${GTEST_IMPORTED_LOCATION})

  # define imported library `libgtest_main`
  add_library(libgtest_main IMPORTED STATIC GLOBAL)
  set_target_properties(
    libgtest_main PROPERTIES INTERFACE_LINK_LIBRARIES libgtest
                             ${GTESTMAIN_IMPORTED_LOCATION})
  add_dependencies(libgtest_main ${GTEST_NAME})

  add_dependencies(${ENGINE_UNITTEST_TARGET} ${GTEST_NAME})
  set(GTEST_LIBRARIES libgtest libgtest_main)
endif(GTEST_FOUND)

target_include_directories(${ENGINE_UNITTEST_TARGET}
                           PRIVATE "${SOREX_ENGINE_DIR}/Include")
target_include_directories(${ENGINE_UNITTEST_TARGET}
                           PRIVATE ${GTEST_INCLUDE_DIR})

# FIXME: libsorex name from variable
message(
  STATUS "[${ENGINE_UNITTEST_TARGET}] Link engine library ${ENGINE_TARGET}")
target_link_libraries(${ENGINE_UNITTEST_TARGET} PRIVATE sorex
                                                        ${GTEST_LIBRARIES})

set(ENGINE_UNITTEST_NAME "CTest_${ENGINE_UNITTEST_TARGET}")
add_test(NAME ${ENGINE_UNITTEST_NAME}
         COMMAND $<TARGET_FILE:${ENGINE_UNITTEST_TARGET}>)

set_property(TEST ${ENGINE_UNITTEST_NAME} PROPERTY LABELS engine)
