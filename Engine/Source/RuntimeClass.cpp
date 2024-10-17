#include <Sorex/RuntimeClass.h>

namespace Sorex
{
  bool RuntimeClass::IsSubclassOf(const RuntimeClass& type) const noexcept
  {
    for (const RuntimeClass* base = _base; base != nullptr; base = base->GetBaseClass())
    {
      if (base->IsSameType(type))
        return true;
    }

    return false;
  }
}
