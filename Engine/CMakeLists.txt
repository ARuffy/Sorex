cmake_minimum_required(VERSION 3.8.0)

include(BuildTools)
include(TargetPlatform)

if(WIN32)
  set(ENGINE_TARGET "Sorex")
else()
  set(ENGINE_TARGET "sorex")
endif()

option(SRXENG_ADDRESS_SANITIZER "Enable address sanitizer for engine code" OFF)
option(SRXENG_OPENGL_DEBUG_OUTPUT "Enable OpenGL Debug Output features" OFF)
option(SRXENG_DISABLE_CHECK "Disable engine check macros: useful for unit tests" OFF)
option(SRXENG_DISABLE_LOGGING "Disable engine logging: useful for unit test" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(${ENGINE_TARGET} STATIC "")
set_target_properties(${ENGINE_TARGET} PROPERTIES LINKER_LANGUAGE CXX)

# ###############################
# Compile Options
# ###############################
sorex_target_compile_options(${ENGINE_TARGET})
get_target_property(compile_flags ${ENGINE_TARGET} COMPILE_OPTIONS)
message(STATUS "[${ENGINE_TARGET}]  Compile options: ${compile_flags}")

# ###############################
# Preprocessor Definitions
# ###############################
sorex_target_compile_definitions(${ENGINE_TARGET})
# TODO: Add Debug level to trace logging
target_compile_definitions(${ENGINE_TARGET}
    PUBLIC
        $<$<CONFIG:Debug>:SOREX_DEBUG=1>
)

if(SRXENG_OPENGL_DEBUG_OUTPUT)
    target_compile_definitions(${ENGINE_TARGET}
        PUBLIC
             $<$<CONFIG:Debug>:ENGINE_OPENGL_DEBUG_OUTPUT>
    )
endif()

if(SRXENG_ADDRESS_SANITIZER)
    message(STATUS "[${ENGINE_TARGET}] Enable AddressSanitizer")

    if(MSVC)
        target_compile_options(${ENGINE_TARGET} PUBLIC /fsanitize=address)
        target_compile_definitions(${ENGINE_TARGET} PUBLIC _DISABLE_VECTOR_ANNOTATION)
        target_compile_definitions(${ENGINE_TARGET} PUBLIC _DISABLE_STRING_ANNOTATION)
    else()
        target_compile_options(${ENGINE_TARGET} PUBLIC -fsanitize=address)
        target_compile_options(${ENGINE_TARGET} PUBLIC -fsanitize=undefined)
        target_link_options(${ENGINE_TARGET} PUBLIC -fsanitize=address)
        target_link_options(${ENGINE_TARGET} PUBLIC -fsanitize=undefined)
    endif()
endif()

if (SRXENG_DISABLE_CHECK)
    message(STATUS "[${ENGINE_TARGET}] Disable check macros")
  target_compile_definitions(${ENGINE_TARGET} PUBLIC SOREX_DISABLE_CHECK)
endif()
if (SRXENG_DISABLE_LOGGING)
    message(STATUS "[${ENGINE_TARGET}] Disable logging macros")
    target_compile_definitions(${ENGINE_TARGET} PUBLIC SOREX_DISABLE_LOGGING)
endif()

target_precompile_headers(${ENGINE_TARGET}
    PUBLIC
    "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_SOURCE_DIR}/Precompiled/Precompiled.h>"
)

target_include_directories(${ENGINE_TARGET} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/Include")
target_include_directories(${ENGINE_TARGET} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/Source")

sorex_target_source(${ENGINE_TARGET} "${CMAKE_CURRENT_LIST_DIR}/Source")
sorex_target_source(${ENGINE_TARGET} "${CMAKE_CURRENT_LIST_DIR}/Include")

# ###############################
# Platform Code
# ###############################
# add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Platform")

# ## Create IDE source tree
get_property(src_files TARGET ${ENGINE_TARGET} PROPERTY SOURCES)
source_group(TREE "${CMAKE_CURRENT_LIST_DIR}" FILES ${src_files})
