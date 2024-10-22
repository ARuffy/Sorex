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

#include <concepts>

#include "CoreMinimal.h"

/**
 * Run-Time Type Information is a simple implementation of rtti system.
 * @note: The RTTI system doesn't support Multiple Inheritance.
 *
 */
namespace Sorex
{
  namespace Concept
  {
    template<typename T>
    concept UnsignedIntegral = std::is_integral_v<T> && std::is_unsigned_v<T>;
  }

  /*
   * TODO: Add documentation
   *
   * TypeInfo
   */
  struct TypeInfo
  {
    template<Concept::UnsignedIntegral T>
    struct Hasher
    {
  private:
      static_assert(sizeof(T) >= sizeof(uint32), "invalid hash type");

      static constexpr T offset = (sizeof(T) <= sizeof(uint32))
                                    ? T{ 2166136261UL }
                                    : T{ 14695981039346656037ULL };
      static constexpr T prime =
        (sizeof(T) <= sizeof(uint32)) ? T{ 16777619UL } : T{ 1099511628211ULL };

  public:
      static constexpr hash_t MakeHash(StringView view) srx_noexcept
      {
        hash_t hash = offset;
        for (auto&& curr : view)
        {
          hash = (hash ^ static_cast<hash_t>(curr)) * prime;
        }
        return hash;
      }
    };

public:
    explicit srx_consteval TypeInfo(const char* str) srx_noexcept: mName(str) {}

    constexpr StringView GetName() const srx_noexcept { return mName; }
    constexpr hash_t     GetHash() const srx_noexcept
    {
      return Hasher<hash_t>::MakeHash(mName);
    }

private:
    StringView mName;
  };

  /**
   * @class RuntimeClass - represent the runtime type of a class.
   */
  class RuntimeClass final
  {
public:
    srx_inline RuntimeClass(const TypeInfo&     typeInfo,
                            const RuntimeClass* base) srx_noexcept
      : mName(typeInfo.GetName())
      , mHash(typeInfo.GetHash())
      , mBase(base)
    {}

    srx_inline RuntimeClass(StringView          name,
                            const hash_t        hash,
                            const RuntimeClass* base) srx_noexcept
      : mName(name)
      , mHash(hash)
      , mBase(base)
    {}

    RuntimeClass(const RuntimeClass&)            = delete;
    RuntimeClass& operator=(const RuntimeClass&) = delete;

    StringView GetName() const srx_noexcept { return mName; }
    hash_t     GetHash() const srx_noexcept { return mHash; }

    bool IsA(const RuntimeClass& type) const srx_noexcept;

    bool operator==(const RuntimeClass& other) const srx_noexcept
    {
      return IsSameType(other);
    }
    bool operator!=(const RuntimeClass& other) const srx_noexcept
    {
      return !IsSameType(other);
    }

private:
    inline const RuntimeClass* GetBaseClass() const srx_noexcept
    {
      return mBase;
    }
    inline bool IsSameType(const RuntimeClass& type) const srx_noexcept
    {
      return this == &type;
    }

private:
    StringView   mName;
    const hash_t mHash;

    const RuntimeClass* const mBase;
  };

  namespace Concept
  {
    template<typename T>
    concept RuntimeClass =
      std::is_polymorphic_v<T>
      && (std::is_same_v<typename T::SorexRttiBase, void>
          || std::is_base_of_v<typename T::SorexRttiBase, T>)
      && (std::is_abstract_v<T>
          || requires(T t) {
               {
                 T::GetTypeInfo()
                 } -> std::convertible_to<Sorex::TypeInfo>;
               {
                 t.GetRuntimeClass()
                 } -> std::same_as<const Sorex::RuntimeClass&>;
             });
  }

  namespace Rtti::Details
  {
    template<Concept::RuntimeClass Class>
    const RuntimeClass& GetOrCreateRuntimeType() srx_noexcept
    {
      if constexpr (!std::is_same_v<typename Class::SorexRttiBase, void>)
      {
        static RuntimeClass rtti{
          Class::GetTypeInfo(),
          std::addressof<const RuntimeClass>(
            GetOrCreateRuntimeType<typename Class::SorexRttiBase>())
        };
        return rtti;
      }

      static const RuntimeClass rtti{ Class::GetTypeInfo(), nullptr };
      return rtti;
    }
  }

  template<Concept::RuntimeClass Class>
  constexpr StringView GetTypeName() srx_noexcept
  {
    return Class::GetTypeInfo().GetName();
  }

  template<Concept::RuntimeClass Class>
  constexpr TypeInfo GetTypeInfo() srx_noexcept
  {
    return Class::GetTypeInfo();
  }

  template<Concept::RuntimeClass Class>
  constexpr const RuntimeClass& GetRuntimeType() srx_noexcept
  {
    return Rtti::Details::GetOrCreateRuntimeType<Class>();
  }

  template<Concept::RuntimeClass DesiredType, Concept::RuntimeClass Type>
  srx_inline
    typename std::enable_if<std::is_base_of_v<Type, DesiredType>, bool>::type
    InstanceOf(const Type& base) srx_noexcept
  {
    return base.GetRuntimeClass().IsA(GetRuntimeType<DesiredType>());
  }

  template<typename Derived, typename Base>
  srx_nodiscard Derived DynamicCast(Base* base) srx_noexcept
  {
    typedef std::remove_cv_t<Base>                           BaseType;
    typedef std::remove_cv_t<std::remove_pointer_t<Derived>> DerivedType;

    static_assert(std::is_pointer_v<Derived>, "Return type must be a pointer");
    static_assert(Concept::RuntimeClass<BaseType>
                    && Concept::RuntimeClass<DerivedType>,
                  "dynamic cast: must be runtime class");

    if constexpr (std::is_base_of<DerivedType, BaseType>::value)
      return static_cast<Derived>(base);

    return (base && InstanceOf<DerivedType>(*base)) ? static_cast<Derived>(base)
                                                    : nullptr;
  }

}  // namespace

#define SRX_RTTI_BASE(TYPE)                                                 \
  private:                                                                  \
  static constexpr ::Sorex::TypeInfo __sorexTypeInfo{ #TYPE };              \
                                                                            \
  public:                                                                   \
  typedef void                       SorexRttiBase;                         \
  static constexpr ::Sorex::TypeInfo GetTypeInfo() srx_noexcept             \
  {                                                                         \
    return __sorexTypeInfo;                                                 \
  }                                                                         \
  template<Sorex::Concept::RuntimeClass Class>                              \
  srx_inline bool IsA() const srx_noexcept                                  \
  {                                                                         \
    return ::Sorex::InstanceOf<Class>(*this);                               \
  }                                                                         \
  virtual const ::Sorex::RuntimeClass& GetRuntimeClass() const srx_noexcept \
  {                                                                         \
    typedef std::remove_const_t<std::remove_pointer_t<decltype(this)>>      \
      ObjectType;                                                           \
    static_assert(std::is_same_v<TYPE, ObjectType>, "invalid object type"); \
    static const ::Sorex::RuntimeClass& rtti =                              \
      ::Sorex::Rtti::Details::GetOrCreateRuntimeType<ObjectType>();         \
    return rtti;                                                            \
  }                                                                         \
                                                                            \
  private:

#define SRX_RTTI(TYPE, BASE)                                           \
  private:                                                             \
  static constexpr ::Sorex::TypeInfo __sorexTypeInfo{ #TYPE };         \
                                                                       \
  public:                                                              \
  typedef BASE                       SorexRttiBase;                    \
  static constexpr ::Sorex::TypeInfo GetTypeInfo() srx_noexcept        \
  {                                                                    \
    return __sorexTypeInfo;                                            \
  }                                                                    \
  virtual const ::Sorex::RuntimeClass& GetRuntimeClass()               \
    const srx_noexcept                 override                        \
  {                                                                    \
    typedef std::remove_const_t<std::remove_pointer_t<decltype(this)>> \
      ObjectType;                                                      \
    static_assert(std::is_base_of_v<BASE, ObjectType>                  \
                    && !std::is_same_v<BASE, ObjectType>,              \
                  "base and derived classes are unrelated");           \
    static const ::Sorex::RuntimeClass& rtti =                         \
      ::Sorex::Rtti::Details::GetOrCreateRuntimeType<ObjectType>();    \
    return rtti;                                                       \
  }                                                                    \
                                                                       \
  private:
