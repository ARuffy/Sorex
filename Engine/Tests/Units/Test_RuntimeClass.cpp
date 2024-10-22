#include "Sorex/Types.h"
#include <gtest/gtest.h>

#include <chrono>

#include <Sorex/RuntimeClass.h>

using namespace Sorex;

#define TEST_MAKE_BASECLASS(NAME)          \
  class Test_##NAME                        \
  {                                        \
    SRX_RTTI_BASE(Test_##NAME)             \
public:                                    \
    virtual ~Test_##NAME()                 \
    {}                                     \
  };                                       \
  static const String kTestString_##NAME = \
    String(String("Test_") + String(#NAME))

#define TEST_MAKE_CLASS(NAME, BASE)        \
  class Test_##NAME: public Test_##BASE    \
  {                                        \
    SRX_RTTI(Test_##NAME, Test_##BASE)     \
public:                                    \
    virtual ~Test_##NAME() override        \
    {}                                     \
  };                                       \
  static const String kTestString_##NAME = \
    String(String("Test_") + String(#NAME))

namespace
{
  TEST_MAKE_BASECLASS(Base1);
  TEST_MAKE_CLASS(Derived11, Base1);
  TEST_MAKE_CLASS(Derived12, Base1);

  TEST_MAKE_CLASS(Child11_1, Derived11);
  TEST_MAKE_CLASS(Child12_1, Derived12);

  TEST_MAKE_CLASS(Child11_2, Child11_1);

  TEST_MAKE_BASECLASS(Base2);


  bool IsDifferentRuntimeClass(const RuntimeClass& lhs,
                               const RuntimeClass& rhs) noexcept
  {
    return lhs != rhs && lhs.GetHash() != rhs.GetHash()
           && lhs.GetName() != rhs.GetName();
  }

  template<typename T, typename Base = typename T::SorexRttiBase>
  bool IsValidHierarchy() noexcept
  {
    if constexpr (!std::is_same_v<T, void> && !std::is_same_v<Base, void>)
    {
      // std::cout << "Check runtime classes: '" << GetTypeName<T>() << "' vs '"
      // << GetTypeName<Base>()
      //           << "'\n";

      TUniquePointer<Base> baseObj = MakeUnique<T>();

      return DynamicCast<T*>(baseObj.get())
             && DynamicCast<const T*>(baseObj.get())
             && GetRuntimeType<T>().IsA(GetRuntimeType<Base>())
             && IsDifferentRuntimeClass(GetRuntimeType<T>(),
                                        GetRuntimeType<Base>())
             && IsValidHierarchy<T, typename Base::SorexRttiBase>()
             && IsValidHierarchy<typename T::SorexRttiBase,
                                 typename Base::SorexRttiBase>();
    }

    return true;
  }
}

TEST(RuntimeClass, CompileTime)
{
  // compile-time check
  constexpr size_t hash_size =
    (static_cast<size_t>(Test_Base1::GetTypeInfo().GetHash()) % 255) + 1;
  unsigned char hash_buf[hash_size] = { 0 };

  constexpr size_t str_size           = GetTypeName<Test_Base1>().length();
  unsigned char    str_bufp[str_size] = { 0 };
}

TEST(RuntimeClass, Base)
{
  EXPECT_EQ(Test_Base1::GetTypeInfo().GetName(), kTestString_Base1);
  EXPECT_EQ(Test_Base1::GetTypeInfo().GetName(),
            StringView(Test_Base1::GetTypeInfo().GetName()));
  EXPECT_EQ(Test_Base1::GetTypeInfo().GetName(), GetTypeName<Test_Base1>());
  EXPECT_NE(Test_Base1::GetTypeInfo().GetHash(), 0);

  Test_Base1 b1;
  EXPECT_EQ(b1.GetRuntimeClass().GetName(), kTestString_Base1);
  EXPECT_EQ(b1.GetRuntimeClass().GetHash(),
            Test_Base1::GetTypeInfo().GetHash());

  Test_Base1 b2;
  EXPECT_EQ(b1.GetRuntimeClass(), b2.GetRuntimeClass());
  EXPECT_EQ(b1.GetRuntimeClass(), GetRuntimeType<Test_Base1>());

  EXPECT_EQ(GetTypeName<Test_Base2>(), kTestString_Base2);
  EXPECT_EQ(GetTypeName<Test_Base2>(),
            StringView(GetTypeInfo<Test_Base2>().GetName()));
  EXPECT_NE(Test_Base2::GetTypeInfo().GetHash(), 0);

  EXPECT_NE(GetRuntimeType<Test_Base1>(), GetRuntimeType<Test_Base2>());
  EXPECT_NE(Test_Base1::GetTypeInfo().GetHash(),
            Test_Base2::GetTypeInfo().GetHash());
  EXPECT_NE(GetTypeName<Test_Base1>(), GetTypeName<Test_Base2>());
}

TEST(RuntimeClass, Derived)
{
  const RuntimeClass& runtime_class = GetRuntimeType<Test_Derived11>();
  EXPECT_EQ(GetTypeName<Test_Derived11>(), kTestString_Derived11);
  EXPECT_EQ(runtime_class.GetName(),
            StringView(Test_Derived11::GetTypeInfo().GetName()));
  EXPECT_EQ(runtime_class.GetHash(), Test_Derived11::GetTypeInfo().GetHash());
  EXPECT_NE(runtime_class.GetHash(), 0);

  Test_Derived11 d1;
  EXPECT_EQ(d1.GetRuntimeClass(), runtime_class);
  EXPECT_EQ(d1.GetRuntimeClass().GetName(), kTestString_Derived11);
  EXPECT_EQ(d1.GetRuntimeClass().GetHash(),
            Test_Derived11::GetTypeInfo().GetHash());

  ASSERT_NE(Test_Base1::GetTypeInfo().GetHash(),
            d1.GetRuntimeClass().GetHash());
}

TEST(RuntimeClass, DynamicCast)
{
  Test_Base1 b1;
  Test_Base1 b2;

  Test_Base1* base_ptr = DynamicCast<Test_Base1*>(&b1);
  EXPECT_NE(base_ptr, nullptr);
  EXPECT_EQ(base_ptr, &b1);

  const Test_Base1* const_base_ptr = DynamicCast<const Test_Base1*>(&b2);
  EXPECT_NE(const_base_ptr, nullptr);
  EXPECT_EQ(const_base_ptr, &b2);

  base_ptr       = nullptr;
  const_base_ptr = nullptr;
  EXPECT_EQ(DynamicCast<Test_Base1*>(base_ptr), nullptr);
  EXPECT_EQ(DynamicCast<const Test_Base1*>(const_base_ptr), nullptr);

  Test_Derived11 d1;
  Test_Base1*    base1 = &d1;
  EXPECT_TRUE(InstanceOf<Test_Derived11>(*base1));

  Test_Derived11* dptr = DynamicCast<Test_Derived11*>(base1);
  EXPECT_NE(dptr, nullptr);
  EXPECT_EQ(dptr, &d1);

  TUniquePointer<Test_Base1> ubase = MakeUnique<Test_Derived11>();
  EXPECT_EQ(ubase->GetRuntimeClass(), d1.GetRuntimeClass());
  EXPECT_TRUE(InstanceOf<Test_Derived11>(*ubase));
  EXPECT_NE(DynamicCast<Test_Derived11*>(ubase.get()), nullptr);
  EXPECT_NE(DynamicCast<const Test_Derived11*>(ubase.get()), nullptr);

  ubase.reset(new Test_Base1());
  EXPECT_NE(ubase->GetRuntimeClass(), d1.GetRuntimeClass());
  EXPECT_FALSE(InstanceOf<Test_Derived11>(*ubase));
  EXPECT_EQ(DynamicCast<Test_Derived11*>(ubase.get()), nullptr);
  EXPECT_EQ(DynamicCast<const Test_Derived11*>(ubase.get()), nullptr);
}

TEST(RuntimeClass, Hierarchy)
{
  EXPECT_TRUE(IsValidHierarchy<Test_Child11_2>());
  EXPECT_TRUE(IsValidHierarchy<Test_Child12_1>());

  TUniquePointer<Test_Base1> ch11 = MakeUnique<Test_Child11_1>();
  TUniquePointer<Test_Base1> ch12 = MakeUnique<Test_Child12_1>();
  EXPECT_TRUE(InstanceOf<Test_Base1>(*ch11));
  EXPECT_TRUE(InstanceOf<Test_Base1>(*ch12));
  EXPECT_TRUE(InstanceOf<Test_Derived11>(*ch11));
  EXPECT_TRUE(InstanceOf<Test_Derived12>(*ch12));

  EXPECT_EQ(DynamicCast<Test_Child12_1*>(ch11.get()), nullptr);
  EXPECT_EQ(DynamicCast<Test_Child11_1*>(ch12.get()), nullptr);
  EXPECT_NE(DynamicCast<Test_Child11_1*>(ch11.get()), nullptr);
  EXPECT_NE(DynamicCast<Test_Child12_1*>(ch12.get()), nullptr);

  EXPECT_EQ(DynamicCast<const Test_Child12_1*>(ch11.get()), nullptr);
  EXPECT_EQ(DynamicCast<const Test_Child11_1*>(ch12.get()), nullptr);
  EXPECT_NE(DynamicCast<const Test_Child11_1*>(ch11.get()), nullptr);
  EXPECT_NE(DynamicCast<const Test_Child12_1*>(ch12.get()), nullptr);
}
// #define SRX_RTTI_TEST_BENCHMARK (1)
#ifdef SRX_RTTI_TEST_BENCHMARK
TEST(RuntimeClass, Benchmark)
{
  constexpr size_t number = 10000000;
  Test_Derived11   d11;
  Test_Child11_2   c11_2;

  Test_Base1* const base_ptr    = &c11_2;
  Test_Base1* const base_to_d11 = &d11;

  volatile Test_Derived11* derived_ptr = nullptr;
  volatile Test_Child11_2* child_ptr   = nullptr;
  volatile Test_Derived11* tmp_derived = nullptr;

  const auto std_begin = std::chrono::steady_clock::now();
  for (size_t i = 0; i < number; ++i)
  {
    derived_ptr = dynamic_cast<Test_Derived11*>(base_ptr);
    child_ptr   = dynamic_cast<Test_Child11_2*>(base_ptr);
    tmp_derived = dynamic_cast<Test_Child11_1*>(base_to_d11);
  }
  auto std_end = std::chrono::steady_clock::now();

  const auto std_time = std_end - std_begin;
  std::cout << "std::dynamic_cast time: " << std_time << std::endl;

  const auto rfy_begin = std::chrono::steady_clock::now();
  for (size_t i = 0; i < number; ++i)
  {
    derived_ptr = DynamicCast<Test_Derived11*>(base_ptr);
    child_ptr   = DynamicCast<Test_Child11_2*>(base_ptr);
    tmp_derived = DynamicCast<Test_Child11_1*>(base_to_d11);
  }
  auto rfy_end = std::chrono::steady_clock::now();

  const auto rfy_time = rfy_end - rfy_begin;
  std::cout << "Sorex::DynamicCast time: " << rfy_time << std::endl;
  EXPECT_LE(rfy_time, std_time);
}
#endif
