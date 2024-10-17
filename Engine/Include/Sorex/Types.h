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

// TODO: debug:
#define srx_inline inline
#define srx_noexcept noexcept
#define srx_consteval consteval
#define srx_nodiscard [[nodiscard]]

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
  srx_nodiscard srx_inline TUniquePointer<T> MakeUnique(Args&&... args)
  {
    return std::make_unique<T>(std::forward<Args>(args)...);
  }

  template<typename T, typename... Args>
  srx_nodiscard srx_inline TSharedPointer<T> MakeShared(Args&&... args)
  {
    return std::make_shared<T>(std::forward<Args>(args)...);
  }

  template<typename... Args>
  using TVariant = std::variant<Args...>;
}
