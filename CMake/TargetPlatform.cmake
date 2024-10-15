cmake_minimum_required(VERSION 3.14)

if(NOT TARGET_PLATFORM)
  message(STATUS "[Lorex] Target platform: '${CMAKE_SYSTEM_NAME}'")

    if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        set(TARGET_PLATFORM "TARGET_PLATFORM_WINDOWS")
        set(TARGET_PLATFORM_WINDOWS TRUE)
    elseif(${CMAKE_SYSTEM_NAME} MATCHES "Android")
        set(TARGET_PLATFORM "TARGET_PLATFORM_ANDROID")
        set(TARGET_PLATFORM_ANDROID TRUE)
    elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        set(TARGET_PLATFORM "TARGET_PLATFORM_LINUX")
        set(TARGET_PLATFORM_LINUX TRUE)
    elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        set(TARGET_PLATFORM "TARGET_PLATFORM_MACOSX")
        set(TARGET_PLATFORM_MACOSX TRUE)
    elseif(${CMAKE_SYSTEM_NAME} MATCHES "iOS")
        set(TARGET_PLATFORM "TARGET_PLATFORM_IOS")
        set(TARGET_PLATFORM_IOS TRUE)
    else()
        message(FATAL_ERROR "Unsupported platform, CMake will exit")
        return()
    endif()
endif()

function(lorex_target_compile_definitions TARGET_NAME)
    message(STATUS "[${TARGET_NAME}] Configuring compile definitions (${CMAKE_SYSTEM_NAME})")

    target_compile_definitions(${TARGET_NAME}
        PUBLIC
            ${TARGET_PLATFORM}=1
            $<$<CONFIG:Debug>:_DEBUG>
            $<$<CONFIG:Release>:NDEBUG>
    )
endfunction(lorex_target_compile_definitions)

function(lorex_target_compile_options TARGET_NAME)

if(MSVC)
    add_compile_options("/permissive-")
    target_compile_options(${TARGET_NAME}
        PRIVATE
            "/WX"
    )
endif()

if(UNIX)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options("-gdwarf-4")
        target_compile_options(${TARGET_NAME}
            PRIVATE
                "-g3"
                "-O0"
                "-Wall"
                "-Werror"
                "-Wextra"
                "-fno-omit-frame-pointer"
                "-Wno-unused-parameter"
        )
    endif()
endif()

endfunction()
