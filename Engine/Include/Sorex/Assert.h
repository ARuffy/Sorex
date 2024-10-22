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

#include "Platform.h"

namespace Sorex::Platform
{
  /**
   * @brief OnAssertionFailed - called when assertion expression failed.
   *    Should be implemented in the platform code;
   *
   * @param message - error message;
   * @param file - file name where  the assertion is used;
   * @param line - line number of the assertion;
   *
   * @return 0 - to continue with assert(expr) to terminate application;
   */
  int OnAssertionFailed(const char* message, const char* file, int line);

  /**
   * @brief OnCheckFailed - called when check/verify expression failed.
   *    Should be implemented in the platform code;
   *
   * @param message - error message;
   * @param file - file name where  the assertion is used;
   * @param line - line number of the assertion;
   */
  void OnCheckFailed(const char* message, const char* file, int line);
}

/**
 * @note: Use assert when it's inpossible or dangerous continue the operating in
 * current context. The `Assert` macro must work in any type of build and
 * terminate program.
 *
 * Failed assertion is wrong thing in any context. So failed assertion during
 * the test is considered as not-passed test.
 */

#ifdef SOREX_DEBUG_NONE
#  define SRX_ASSERT(expr)
#  define SRX_ASSERT_MSG(expr, msg)
#else
#  define SRX_ASSERT(expr)                                                     \
    ((void)((!!(expr))                                                         \
            || ::Sorex::Platform::OnAssertionFailed(#expr, __FILE__, __LINE__) \
            || (assert(expr), 0)))

#  define SRX_ASSERT_MSG(expr, msg)                                          \
    ((void)((!!(expr))                                                       \
            || ::Sorex::Platform::OnAssertionFailed(msg, __FILE__, __LINE__) \
            || (assert(expr), 0)))
#endif

/**
 * @note: Verify macros
 * Use when you want to inform that something is going wrong but the context
 * allows us to continue work. The expresion inside the macro should be invoked
 * in any type of build. It allows to programmer put a function call inside the
 * macro.
 *
 * Some test can examine state where a verify macro can fail. For expample:
 * invalid argument to check that context stay stable. So, if the
 * SRXENG_DISABLE_CHECK macro define the verify macro should only invoke
 * expression.
 */
#ifdef SOREX_DEBUG_MEDIUM
#  define SRX_VERIFY(expr)                                         \
    if (SRX_UNLIKELY(!(expr)))                                     \
      ::Sorex::Platform::OnCheckFailed(#expr, __FILE__, __LINE__); \
    else                                                           \
      ((void)0)

#  define SRX_VERIFY_MSG(expr, msg)                                \
    if (SRX_UNLIKELY(!(expr)))                                     \
      ::Sorex::Platform::OnCheckFailed(#expr, __FILE__, __LINE__); \
    else                                                           \
      ((void)0)
#else
#  define SRX_VERIFY(expr) ((void)(expr))
#  define SRX_VERIFY_MSG(expr, msg) ((void)(expr))

#endif

/**
 * @note: Check macros
 * Use checking when you want inform that something is going wrong but the
 * context allows us to continue work. In release built checking macros can be
 * disabled at all. So don't put sensitive operation inside the macro (exmp.
 * function call), because in different built the behavior of context will be
 * changed.
 *
 * Some test can examine state where a check macro can fail. For expample:
 * invalid argument to check that context stay stable. In this case, if the
 * SRXENG_DISABLE_CHECK macro is defined the checking macro should do
 * nothing.
 */
#ifdef SOREX_DEBUG_MEDIUM
#  define SRX_CHECK(expr) SRX_VERIFY(expr)
#  define SRX_CHECK_MSG(expr, msg) SRX_VERIFY_MSG(expr, msg)

#  define SRX_NOENTRY(msg) \
    ((void)(::Sorex::Platform::Linux::OnCheckFailed(msg, __FILE__, __LINE__)))

#else
#  define SRX_CHECK(expr)
#  define SRX_CHECK_MSG(expr, msg)
#  define SRX_NOENTRY(msg)
#endif
