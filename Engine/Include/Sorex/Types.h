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

#include <cstdint>
#include <string>
#include <vector>
#include <array>
#include <stack>
#include <queue>
#include <list>
#include <map>
#include <set>
#include <forward_list>
#include <unordered_map>
#include <unordered_set>
#include <span>
#include <memory>
#include <variant>

#define SRX_IDLE \
  do             \
  {              \
  } while (0)

namespace Sorex
{
  typedef std::int8_t  int8;
  typedef std::int16_t int16;
  typedef std::int32_t int32;
  typedef std::int64_t int64;

  typedef unsigned char byte;

  typedef std::uint8_t  uint8;
  typedef std::uint16_t uint16;
  typedef std::uint32_t uint32;
  typedef std::uint64_t uint64;

  typedef std::intptr_t intptr_t;

  typedef std::size_t size_t;
  typedef size_t      hash_t;

  typedef int errno_t;

  template<typename T>
  using THash = std::hash<T>;

  typedef std::string  String;
  typedef std::wstring WString;

  template<typename Char>
  using BasicString = std::basic_string<Char>;

  typedef std::string_view  StringView;
  typedef std::wstring_view WStringView;

  template<typename Char>
  using BasicStringView = std::basic_string_view<Char>;

  template<typename T>
  using TVector = std::vector<T>;
  template<typename T, size_t N>
  using TArray = std::array<T, N>;
  template<typename T>
  using TStack = std::stack<T>;
  template<typename T>
  using TQueue = std::queue<T>;
  template<typename T>
  using TPriorityQueue = std::priority_queue<T>;
  template<typename T>
  using TSpan = std::span<T>;

  template<typename T>
  using TSet = std::set<T>;
  template<typename T, typename Hasher = std::hash<T>>
  using THashSet = std::unordered_set<T, Hasher>;

  template<typename TKey, typename TValue>
  using TMap = std::map<TKey, TValue>;
  template<typename TKey, typename TValue>
  using THashMap = std::unordered_map<TKey, TValue>;

  template<typename T>
  using TList = std::list<T>;
  template<typename T>
  using TLinkedList = std::forward_list<T>;

  template<typename TFirst, typename TSecond>
  using TPair = std::pair<TFirst, TSecond>;

  template<typename T>
  using TUniquePointer = std::unique_ptr<T>;
  template<typename T>
  using TSharedPointer = std::shared_ptr<T>;
  template<typename T>
  using TWeakPointer = std::weak_ptr<T>;

  template<typename T, typename... Args>
  [[nodiscard]] inline TUniquePointer<T> MakeUnique(Args&&... args)
  {
    return std::make_unique<T>(std::forward<Args>(args)...);
  }

  template<typename T, typename... Args>
  [[nodiscard]] inline TSharedPointer<T> MakeShared(Args&&... args)
  {
    return std::make_shared<T>(std::forward<Args>(args)...);
  }

  template<typename... Args>
  using TVariant = std::variant<Args...>;
}
