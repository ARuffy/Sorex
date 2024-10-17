#pragma once

#include <concepts>

#include "Types.h"

/**
 * Run-Time Type Information is a simple implementation of rtti system.
 * @note: The RTTI system doesn't support Multiple Inheritance.
 *
 */
namespace Sorex
{
  namespace Details
  {
    template<typename = hash_t>
    struct fnv_1a_params;

    template<>
    struct fnv_1a_params<uint32_t>
    {
      static constexpr auto offset = 2166136261;
      static constexpr auto prime  = 16777619;
    };

    template<>
    struct fnv_1a_params<uint64_t>
    {
      static constexpr auto offset = 14695981039346656037ULL;
      static constexpr auto prime  = 1099511628211ULL;
    };
  }  // namespace

  /*
     * TODO: Add documentation
     *
     * TypeInfo
    */
  struct TypeInfo
  {
public:
    explicit srx_consteval TypeInfo(const char* str) srx_noexcept: mName(str) {}

    constexpr hash_t     GetHash() const srx_noexcept { return MakeHash(mName); }
    constexpr StringView GetName() const srx_noexcept { return mName; }

private:
    static constexpr hash_t MakeHash(StringView view) srx_noexcept
    {
      using params = Details::fnv_1a_params<>;
      hash_t hash  = params::offset;
      for (auto&& curr : view)
      {
        hash = (hash ^ static_cast<hash_t>(curr)) * params::prime;
      }
      return hash;
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
    srx_inline RuntimeClass(const TypeInfo& typeInfo, const RuntimeClass* base) srx_noexcept
      : _name(typeInfo.GetName())
      , _hash(typeInfo.GetHash())
      , _base(base)
    {}

    srx_inline RuntimeClass(StringView name, const hash_t hash, const RuntimeClass* base) srx_noexcept
      : _name(name)
      , _hash(hash)
      , _base(base)
    {}

    RuntimeClass(const RuntimeClass&)            = delete;
    RuntimeClass& operator=(const RuntimeClass&) = delete;

    StringView GetName() const srx_noexcept { return _name; }
    hash_t     GetHash() const srx_noexcept { return _hash; }

    bool IsA(const RuntimeClass& type) const srx_noexcept { return IsSameType(type) || IsSubclassOf(type); }

    bool operator==(const RuntimeClass& other) const srx_noexcept { return IsSameType(other); }
    bool operator!=(const RuntimeClass& other) const srx_noexcept { return !IsSameType(other); }

private:
    const RuntimeClass* GetBaseClass() const srx_noexcept { return _base; }

    bool IsSameType(const RuntimeClass& type) const srx_noexcept { return this == &type; }
    bool IsSubclassOf(const RuntimeClass& type) const srx_noexcept;

private:
    StringView   _name;
    const hash_t _hash;

    const RuntimeClass* const _base;
  };

  namespace Concept
  {
    template<typename T>
    concept RuntimeClass =
      std::is_polymorphic_v<T>
      && (std::is_same_v<typename T::Base, void> || std::is_base_of_v<typename T::Base, T>)
      && (std::is_abstract_v<T> || requires(T t) {
           { T::GetTypeInfo() } -> std::convertible_to<Sorex::TypeInfo>;
           { t.GetRuntimeClass() } -> std::same_as<const Sorex::RuntimeClass&>;
         });
  }

  namespace Details
  {
    template<Concept::RuntimeClass Class>
    const RuntimeClass& GetOrCreateRuntimeType() srx_noexcept
    {
      if constexpr (!std::is_same_v<typename Class::Base, void>)
      {
        static RuntimeClass rtti{ Class::GetTypeInfo(),
                                  std::addressof<const RuntimeClass>(
                                    GetOrCreateRuntimeType<typename Class::Base>()) };
        return rtti;
      }

      static RuntimeClass rtti{ Class::GetTypeInfo(), nullptr };
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
    return Details::GetOrCreateRuntimeType<Class>();
  }

  template<Concept::RuntimeClass DesiredType, Concept::RuntimeClass Type>
  srx_inline typename std::enable_if<std::is_base_of_v<Type, DesiredType>, bool>::type InstanceOf(
    const Type& base) srx_noexcept
  {
    return base.GetRuntimeClass().IsA(GetRuntimeType<DesiredType>());
  }

  template<typename Derived, typename Base>
  srx_nodiscard Derived DynamicCast(Base* base) srx_noexcept
  {
    typedef std::remove_cv_t<Base>                           BaseType;
    typedef std::remove_cv_t<std::remove_pointer_t<Derived>> DerivedType;

    static_assert(std::is_pointer_v<Derived>, "Return type must be a pointer");
    static_assert(Concept::RuntimeClass<BaseType> && Concept::RuntimeClass<DerivedType>,
                  "dynamic cast: must be runtime class");

    if constexpr (std::is_base_of<DerivedType, BaseType>::value)
      return static_cast<Derived>(base);

    return (base && InstanceOf<DerivedType>(*base)) ? static_cast<Derived>(base) : nullptr;
  }

}  // namespace

#define SRX_RTTI_BASE(TYPE)                                                                          \
  private:                                                                                           \
  srx_inline static constexpr Sorex::TypeInfo __sorexTypeInfo{ #TYPE };                              \
                                                                                                     \
  public:                                                                                            \
  typedef void                     Base;                                                             \
  static constexpr Sorex::TypeInfo GetTypeInfo() srx_noexcept                                        \
  {                                                                                                  \
    return __sorexTypeInfo;                                                                          \
  }                                                                                                  \
  template<Sorex::Concept::RuntimeClass Class>                                                       \
  srx_inline bool IsA() const srx_noexcept                                                           \
  {                                                                                                  \
    return Sorex::InstanceOf<Class>(*this);                                                          \
  }                                                                                                  \
  virtual const Sorex::RuntimeClass& GetRuntimeClass() const srx_noexcept                            \
  {                                                                                                  \
    typedef std::remove_const_t<std::remove_pointer_t<decltype(this)>> ObjectType;                   \
    static_assert(std::is_same_v<TYPE, ObjectType>, "invalid object type");                          \
    static const Sorex::RuntimeClass& rtti = ::Sorex::Details::GetOrCreateRuntimeType<ObjectType>(); \
    return rtti;                                                                                     \
  }                                                                                                  \
                                                                                                     \
  private:

#define SRX_RTTI(TYPE, BASE)                                                                         \
  private:                                                                                           \
  srx_inline static constexpr Sorex::TypeInfo __sorexTypeInfo{ #TYPE };                              \
                                                                                                     \
  public:                                                                                            \
  typedef BASE                     Base;                                                             \
  static constexpr Sorex::TypeInfo GetTypeInfo() srx_noexcept                                        \
  {                                                                                                  \
    return __sorexTypeInfo;                                                                          \
  }                                                                                                  \
  virtual const Sorex::RuntimeClass& GetRuntimeClass() const srx_noexcept override                   \
  {                                                                                                  \
    typedef std::remove_const_t<std::remove_pointer_t<decltype(this)>> ObjectType;                   \
    static_assert(std::is_base_of_v<BASE, ObjectType> && !std::is_same_v<BASE, ObjectType>,          \
                  "base and derived classes are unrelated");                                         \
    static const Sorex::RuntimeClass& rtti = ::Sorex::Details::GetOrCreateRuntimeType<ObjectType>(); \
    return rtti;                                                                                     \
  }                                                                                                  \
                                                                                                     \
  private:
