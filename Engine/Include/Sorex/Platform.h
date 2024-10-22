/**************************************************************************/
/*                         This file is part of:                          */
/*                                SOREX                                   */
/*                 Simple OpenGL Rendering Engine eXtended                */
/**************************************************************************/
/* Copyright (c) 2022 Aleksandr Ershov (Ruffy).                           */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#pragma once

namespace Sorex
{
  /**
   * @brief Enum class representing the debug mode of the application.
   *
   * The debug mode determines which checking functionalities are enabled,
   * including logging, assertions, verification, and validation.
   */
  enum class EDebugMode
  {
    kNone = 0,    ///< No checks are performed at all.
    kLow  = 1,    ///< Non-performance intensive checks: assertion macros,
                  ///< and basic logging (info, warn, error).
    kMedium = 2,  ///< Performance-intensive checks: check/verify macros,
                  ///< debug logging, and other checks.
    kHigh = 3     ///< Intensive validation with no concerns for performance.
  };

  enum class ETargetPlatform
  {
    kWin32,
    kLinux,
    kMacOS,
    kAppleIOS,
    kAndroid
  };

  enum class ESorexCompiler
  {
    MSVC,
    GNUC,
    GCCE,
    CLANG
  };
}

#if !defined(SOREX_DEBUG_MODE)
#  define SOREX_DEBUG_LOW (1)
#else
#  if SOREX_DEBUG_MODE == 0
#    define SOREX_DEBUG_NONE (1)
#  else
#    if SOREX_DEBUG_MODE >= 1  // ::Sorex::EDebugMode::kLow
#      define SOREX_DEBUG_LOW (1)
#    endif
#    if SOREX_DEBUG_MODE >= 2  // ::Sorex::EDebugMode::kMedium
#      define SOREX_DEBUG_MEDIUM (1)
#    endif
#    if SOREX_DEBUG_MODE >= 3  //::Sorex::EDebugMode::kHigh
#      define SOREX_DEBUG_HIGH (1)
#    endif
#  endif
#endif

// Finds current platform
#if defined(__WIN32__) || defined(_WIN32) || defined(TARGET_PLATFORM_WINDOWS)
#  define SOREX_TARGET_PLATFORM (::Sorex::ETargetPlatform::kWin32)
#  define SOREX_PLATFORM_WIN32 (1)

#elif defined(__ANDROID__) || defined(TARGET_PLATFORM_ANDROID)
#  define SOREX_TARGET_PLATFORM (::Sorex::ETargetPlatform::kAndroid)
#  define SOREX_PLATFORM_ANDROID (1)

#elif defined(TARGET_PLATFORM_LINUX)
#  define SOREX_TARGET_PLATFORM (::Sorex::ETargetPlatform::kLinux)
#  define SOREX_PLATFORM_LINUX (1)

#elif defined(__APPLE_CC__) || defined(TARGET_PLATFORM_MACOSX) \
  || defined(TARGET_PLATFORM_IOS)
#  if __ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__ >= 60000 \
    || __IPHONE_OS_VERSION_MIN_REQUIRED >= 60000
#    define SOREX_TARGET_PLATFORM (::Sorex::ETargetPlatform::kAppleIOS)
#    define SOREX_PLATFORM_IOS (1)
#  else
#    define SOREX_TARGET_PLATFORM (::Sorex::ETargetPlatform::kMacOS)
#    define SOREX_PLATFORM_MACOSX (1)
#  endif

#else
#  pragma error "Sorex doesn't support target platform!"
#endif

// Finds the compiler type and version
#if defined(__GCCE__)
#  define SOREX_COMPILER (::Sorex::ESorexCompiler::GCCE)
#  define SOREX_COMPILER_GCCE (1)
#  define SOREX_COMP_VER _MSC_VER

#elif defined(_MSC_VER)
#  define SOREX_COMPILER (::Sorex::ESorexCompiler::MSVC)
#  define SOREX_COMPILER_MSVC (1)
#  define SOREX_COMP_VER _MSC_VER

#elif defined(__clang__)
#  define SOREX_COMPILER (::Sorex::ESorexCompiler::CLANG)
#  define SOREX_COMPILER_CLANG (1)
#  define SOREX_COMP_VER \
    (((__clang_major__)*100) + (__clang_minor__ * 10) + __clang_patchlevel__)

#elif defined(__GNUC__)
#  define SOREX_COMPILER (::Sorex::ESorexCompiler::GNUC)
#  define SOREX_COMPILER_GNUC (1)
#  define SOREX_COMP_VER \
    (((__GNUC__)*100) + (__GNUC_MINOR__ * 10) + __GNUC_PATCHLEVEL__)

#else
#  pragma error "Sorex unknown compiler. Abort!"
#endif

// Windows Settings
#ifdef SOREX_PLATFORM_WIN32
#  if defined(SOREX_STATIC_LIB)
#    define SRX_API
#  else
#    if defined(SOREX_EXPORT_API)
#      define SRX_API __declspec(dllexport)
#    else
#      define SRX_API __declspec(dllimport)
#    endif
#  endif  // defined(SOREX_STATIC_LIB)
#endif    // SOREX_PLATFORM == SOREX_PLATFORM_WIN32

// Function signature macros
#ifdef SOREX_DEBUG_MEDIUM
#  define srx_inline
#else
#  if defined(SOREX_COMPILER_MSVC) && SOREX_COMP_VER >= 1200
#    define srx_inline __forceinline

#  elif !defined(__ANDROID__) \
    && (defined(SOREX_COMPILER_GNUC) || defined(SOREX_COMPILER_CLANG))
#    define srx_inline inline __attribute__((always_inline))

#  else
#    define srx_inline inline
#  endif
#endif

#define srx_noexcept noexcept
#define srx_consteval consteval
#define srx_nodiscard [[nodiscard]]

// Macros
#ifdef SOREX_COMPILER_MSVC
#  define SRX_TYPENAME typename
#else
#  define SRX_TYPENAME
#endif

#define SRX_LIKELY [[likely]]
#define SRX_UNLIKELY [[unlikely]]
