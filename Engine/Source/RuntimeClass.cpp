#include <Sorex/RuntimeClass.h>

namespace Sorex
{
  bool RuntimeClass::IsA(const RuntimeClass& type) const srx_noexcept
  {
    if (IsSameType(type))
      return true;

    // check the class hirarchy
    const RuntimeClass* ptrType = mBase;
    while (ptrType)
    {
      if (ptrType->IsSameType(type))
        return true;

      ptrType = ptrType->GetBaseClass();
    }

    return false;
  }
}
