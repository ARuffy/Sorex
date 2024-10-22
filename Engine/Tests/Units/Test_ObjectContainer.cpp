#include <gtest/gtest.h>

#include <Sorex/Types.h>
#include <Sorex/Containers/ObjectContainer.h>

namespace
{
  const Sorex::String kBaseClassName    = "Base";
  const Sorex::String kDerivedClassName = "Derived";
}

using namespace Sorex;

class TestComponentBase
{
  SRX_RTTI_BASE(TestComponentBase)

  public:
  virtual ~TestComponentBase() = default;
  TestComponentBase()
    : _name(kBaseClassName)
    , _value(0)
  {}

  const String& GetName() const { return _name; }
  int           GetNumber() const { return _value; }

  void Print() const
  {
    std::cout << "[" << _name << "]" << " Component value: " << _name
              << std::endl;
  }

  protected:
  String _name;
  int    _value = 0;
};

class TestComponent: public TestComponentBase
{
  SRX_RTTI(TestComponent, TestComponentBase)

  public:
  explicit TestComponent(const String& name, int value = 1)
    : TestComponentBase()
  {
    _name  = name;
    _value = value;
  }
};


TEST(ObjectContainer, Main)
{
  // Add
  TObjectContainer<TestComponentBase> components;
  ASSERT_TRUE(components.IsEmpty());
  ASSERT_EQ(components.GetSize(), 0u);

  TestComponentBase* base = components.Add(MakeUnique<TestComponentBase>());
  ASSERT_NE(base, nullptr);
  ASSERT_FALSE(components.IsEmpty());
  ASSERT_EQ(components.GetSize(), 1u);
  ASSERT_TRUE(components.Contains(base));

  TestComponent* cmp1 = components.Add<TestComponent>(kDerivedClassName, 1);
  ASSERT_NE(cmp1, nullptr);
  ASSERT_FALSE(components.IsEmpty());
  ASSERT_EQ(components.GetSize(), 2u);
  ASSERT_TRUE(components.Contains(cmp1));

  TestComponent* cmp2 = components.Add<TestComponent>(kDerivedClassName, 2);
  ASSERT_NE(cmp2, nullptr);
  ASSERT_FALSE(components.IsEmpty());
  ASSERT_EQ(components.GetSize(), 3u);
  ASSERT_TRUE(components.Contains(cmp2));

  TestComponent* cmp3 = components.Add<TestComponent>(kDerivedClassName, 3);
  ASSERT_NE(cmp3, nullptr);
  ASSERT_FALSE(components.IsEmpty());
  ASSERT_EQ(components.GetSize(), 4u);
  ASSERT_TRUE(components.Contains(cmp3));

  TestComponentBase* base2 = components.Add(MakeUnique<TestComponentBase>());
  ASSERT_NE(base2, nullptr);
  ASSERT_FALSE(components.IsEmpty());
  ASSERT_EQ(components.GetSize(), 5u);
  ASSERT_TRUE(components.Contains(base2));

  // Get
  TestComponentBase* baseCpy = components.Get<TestComponentBase>();
  ASSERT_NE(baseCpy, nullptr);
  EXPECT_EQ(baseCpy, base);
  EXPECT_NE(baseCpy, base2);

  TestComponent* cmp1Cpy = components.Get<TestComponent>();
  ASSERT_NE(cmp1Cpy, nullptr);
  EXPECT_EQ(cmp1Cpy, cmp1);
  EXPECT_EQ(cmp1Cpy->GetNumber(), cmp1->GetNumber());

  const auto&          constComponents = components;
  const TestComponent* cmp1ConstCpy    = constComponents.Get<TestComponent>();
  ASSERT_NE(cmp1ConstCpy, nullptr);
  EXPECT_EQ(cmp1ConstCpy, cmp1Cpy);
  EXPECT_EQ(cmp1ConstCpy->GetNumber(), cmp1Cpy->GetNumber());

  TVector<const TestComponentBase*> vecBase;
  constComponents.GetAll(vecBase);
  EXPECT_EQ(vecBase.size(), constComponents.GetSize());
  for (const TestComponentBase* cmp : vecBase)
  {
    ASSERT_NE(cmp, nullptr);
  }

  TVector<TestComponent*> vecDerived;
  components.GetAll(vecDerived);
  EXPECT_EQ(vecDerived.size(), 3u);
  for (TestComponent* cmp : vecDerived)
  {
    ASSERT_NE(cmp, nullptr);
    EXPECT_EQ(cmp->GetName(), kDerivedClassName);
  }

  // Release
  auto uptr = components.Release(cmp3);
  ASSERT_NE(uptr.get(), nullptr);
  EXPECT_TRUE(uptr->GetName() == kDerivedClassName);
  EXPECT_TRUE(uptr->GetNumber() == 3);
  ASSERT_FALSE(components.IsEmpty());
  ASSERT_EQ(components.GetSize(), 4u);
  cmp3 = nullptr;

  uptr.reset();
  uptr = components.Release<TestComponent>();
  ASSERT_NE(uptr.get(), nullptr);
  EXPECT_TRUE(uptr->GetName() == kDerivedClassName);
  EXPECT_TRUE(uptr->GetNumber() == 1);
  ASSERT_FALSE(components.IsEmpty());
  ASSERT_EQ(components.GetSize(), 3u);
  cmp1 = nullptr;

  // Remove
  EXPECT_TRUE(components.Contains(cmp2));
  EXPECT_TRUE(components.Remove(cmp2));
  cmp2 = nullptr;

  ASSERT_FALSE(components.IsEmpty());
  ASSERT_EQ(components.GetSize(), 2u);
  ASSERT_TRUE(components.Contains(base));

  TestComponent tmp(kDerivedClassName, 123);
  EXPECT_FALSE(components.Remove(nullptr));
  EXPECT_FALSE(components.Remove(&tmp));

  components.RemoveAll<TestComponentBase>();
  base = nullptr;
  ASSERT_TRUE(components.IsEmpty());
  ASSERT_EQ(components.GetSize(), 0u);
}
