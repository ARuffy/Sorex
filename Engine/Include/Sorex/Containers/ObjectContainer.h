#pragma once

#include <type_traits>

#include <Sorex/Types.h>
#include <Sorex/RuntimeClass.h>

namespace Sorex
{
  /**
   * @class TObjectContainer.
   *
   * @brief TObjectContainer contains and manages the object by their type.
   */
  template<class Type>
  class TObjectContainer
  {
    template<typename T>
    using TEnableIf_IsSubclassType =
      typename std::enable_if<std::is_base_of<Type, T>::value, T>::type;

public:
    using Container     = TVector<TUniquePointer<Type>>;
    using Iterator      = typename Container::iterator;
    using ConstIterator = typename Container::const_iterator;

    TObjectContainer(const TObjectContainer&)            = delete;
    TObjectContainer& operator=(const TObjectContainer&) = delete;

    TObjectContainer() = default;
    virtual ~TObjectContainer() { Clear(); }

    Iterator begin() { return _container.begin(); }
    Iterator end() { return _container.end(); }

    ConstIterator cbegin() const { return _container.cbegin(); }
    ConstIterator cend() const { return _container.cend(); }

    /**
     * @param object - the object pointer
     *
     * @return True if the contanier has the object
     */
    srx_inline bool Contains(const Type* object) const srx_noexcept;

    /**
     * @return true if the container is empty
     */
    bool IsEmpty() const srx_noexcept { return _container.empty(); }

    /**
     * @return number of objects that are stored in container
     */
    size_t GetSize() const srx_noexcept { return _container.size(); }

    /**
     * Move a new object to container. The TObjectContainer owns and manages the
     * object.
     *
     * @param object - move object to container
     * @return pointer to the object or NULL if object isn't valid.
     */
    Type* Add(TUniquePointer<Type>&& object) srx_noexcept;

    /**
     * Create and added new object to container. The TObjectContainer owns and
     * manages the object.
     *
     * @param args - arguments to construct new object
     * @return pointer to created object or NULL if object wasn't created.
     */
    template<class T, typename... Args>
    srx_typename TEnableIf_IsSubclassType<T>* Add(Args&&... args);

    /**
     * Release the object from container by pointer.
     *
     * @param object - pointer to a removing object
     * @return unique pointer that points to the removed object or NULL if
     * object wasn't found.
     */
    template<class T, class Enable = srx_typename TEnableIf_IsSubclassType<T>>
    srx_nodiscard TUniquePointer<T> Release(const T* object) srx_noexcept;

    /**
     * Release the first found object from container by type.
     *
     * @return unique pointer that points to the removed object or NULL if
     * object wasn't found.
     */
    template<class T, class Enable = srx_typename TEnableIf_IsSubclassType<T>>
    srx_nodiscard TUniquePointer<T> Release() srx_noexcept;

    /**
     * Remove the object from the container and destroy it
     *
     * @return True if element was deleted
     */
    bool Remove(const Type* object) srx_noexcept;

    /**
     * Remove all objects with the type from the container and destroy them
     */
    template<class T, class Enable = srx_typename TEnableIf_IsSubclassType<T>>
    void RemoveAll() srx_noexcept;

    /**
     * Delete all objects and clear container
     */
    void Clear() srx_noexcept;

    /**
     * @return first found object by type
     */
    template<class T, class Enable = srx_typename TEnableIf_IsSubclassType<T>>
    const T* Get() const srx_noexcept;

    /**
     * @return first found object by type
     */
    template<class T>
    srx_typename TEnableIf_IsSubclassType<T>* Get() srx_noexcept;

    /**
     * @return all found object by type
     */
    template<class T, class Enable = srx_typename TEnableIf_IsSubclassType<T>>
    srx_inline void GetAll(TVector<T*>& objects) srx_noexcept;

    /**
     * @return all found object by type
     */
    template<class T, class Enable = srx_typename TEnableIf_IsSubclassType<T>>
    srx_inline void GetAll(TVector<const T*>& objects) const srx_noexcept;

private:
    template<class T>
    void GetAll(TVector<T*>& objects, const RuntimeClass& type) srx_noexcept;
    template<class T>
    void GetAll(TVector<const T*>&  objects,
                const RuntimeClass& type) const srx_noexcept;

    srx_inline Iterator      Find(const Type* object) srx_noexcept;
    srx_inline ConstIterator Find(const Type* object) const srx_noexcept;

    srx_inline Iterator      Find(const RuntimeClass& type) srx_noexcept;
    srx_inline ConstIterator Find(const RuntimeClass& type) const srx_noexcept;
    template<class T>
    srx_inline Iterator Find() srx_noexcept;

private:
    Container _container;
  };

  template<class Type>
  srx_inline bool TObjectContainer<Type>::Contains(const Type* object) const
    srx_noexcept
  {
    return Find(object) != _container.cend();
  }

  template<class Type>
  Type* TObjectContainer<Type>::Add(TUniquePointer<Type>&& object) srx_noexcept
  {
    if (!object)
      return nullptr;

    Type* result = object.get();
    _container.push_back(std::move(object));

    return result;
  }

  template<class Type>
  template<class T, typename... Args>
  typename TObjectContainer<Type>::template TEnableIf_IsSubclassType<T>*
  TObjectContainer<Type>::Add(Args&&... args)
  {
    return static_cast<T*>(Add(MakeUnique<T>(std::forward<Args>(args)...)));
  }

  template<class Type>
  template<class T, class Enable>
  srx_nodiscard TUniquePointer<T> TObjectContainer<Type>::Release(
    const T* object) srx_noexcept
  {
    if (auto it = Find(object); it != _container.end())
    {
      T* const ptr = static_cast<T*>(it->release());
      _container.erase(it);
      return TUniquePointer<T>(ptr);
    }

    return nullptr;
  }

  template<class Type>
  template<class T, class Enable>
  srx_nodiscard TUniquePointer<T> TObjectContainer<Type>::Release() srx_noexcept
  {
    const RuntimeClass& type = GetRuntimeType<T>();
    if (Iterator it = Find(type); it != _container.end())
    {
      T* const ptr = static_cast<T*>(it->release());
      _container.erase(it);
      return TUniquePointer<T>(ptr);
    }

    return nullptr;
  }

  template<class Type>
  bool TObjectContainer<Type>::Remove(const Type* object) srx_noexcept
  {
    if (Iterator it = Find(object); it != _container.end())
    {
      _container.erase(it);
      return true;
    }

    return false;
  }

  template<class Type>
  template<class T, class Enable>
  void TObjectContainer<Type>::RemoveAll() srx_noexcept
  {
    const RuntimeClass& cls = GetRuntimeType<T>();
    _container.erase(
      std::remove_if(_container.begin(),
                     _container.end(),
                     [&](const TUniquePointer<Type>& item) {
                       return item && item->GetRuntimeClass().IsA(cls);
                     }),
      _container.end());
  }

  template<class Type>
  void TObjectContainer<Type>::Clear() srx_noexcept
  {
    // NOTE: we must delete all object in reverse order.
    // In this case we guarantee that injected dependencies work properly during
    // the objects destruction.
    for (auto it = _container.rbegin(); it != _container.rend(); ++it)
      it->reset();

    _container.clear();
  }

  template<class Type>
  template<class T, class Enable>
  const T* TObjectContainer<Type>::Get() const srx_noexcept
  {
    if (ConstIterator it = Find(GetRuntimeType<T>()); it != _container.cend())
      return static_cast<T*>(it->get());

    return nullptr;
  }

  template<class Type>
  template<class T>
  typename TObjectContainer<Type>::template TEnableIf_IsSubclassType<T>*
  TObjectContainer<Type>::Get() srx_noexcept
  {
    if (Iterator it = Find(GetRuntimeType<T>()); it != _container.end())
      return static_cast<T*>(it->get());

    return nullptr;
  }

  template<class Type>
  template<class T, class Enable>
  srx_inline void TObjectContainer<Type>::GetAll(
    TVector<const T*>& objects) const srx_noexcept
  {
    return GetAll<T>(objects, GetRuntimeType<T>());
  }

  template<class Type>
  template<class T>
  srx_inline void TObjectContainer<Type>::GetAll(TVector<T*>& objects)
    srx_noexcept
  {
    return GetAll<T>(objects, GetRuntimeType<T>());
  }

  template<class Type>
  template<class T>
  void TObjectContainer<Type>::GetAll(TVector<const T*>&  objects,
                                      const RuntimeClass& type) const
    srx_noexcept
  {
    static_assert(std::is_base_of<Type, T>::value,
                  "[TObjectContainer] GetAll objects of invalid runtime class");

    objects.clear();
    objects.reserve(_container.size());

    for (ConstIterator it = _container.cbegin(); it != _container.cend(); ++it)
    {
      if (const TUniquePointer<Type>& item = (*it);
          item && item->GetRuntimeClass().IsA(type))
        objects.push_back(static_cast<const T*>(item.get()));
    }
  }

  template<class Type>
  template<class T>
  void TObjectContainer<Type>::GetAll(TVector<T*>&        objects,
                                      const RuntimeClass& type) srx_noexcept
  {
    static_assert(std::is_base_of<Type, T>::value,
                  "[TObjectContainer] GetAll objects of invalid runtime class");

    objects.clear();
    objects.reserve(_container.size());

    for (Iterator it = _container.begin(); it != _container.end(); ++it)
    {
      if (const TUniquePointer<Type>& item = (*it);
          item && item->GetRuntimeClass().IsA(type))
        objects.push_back(static_cast<T*>(item.get()));
    }
  }

  template<class Type>
  srx_inline typename TObjectContainer<Type>::Iterator
  TObjectContainer<Type>::Find(const Type* object) srx_noexcept
  {
    return std::find_if(_container.begin(),
                        _container.end(),
                        [object](const TUniquePointer<Type>& element) {
                          return element.get() == object;
                        });
  }

  template<class Type>
  srx_inline typename TObjectContainer<Type>::ConstIterator
  TObjectContainer<Type>::Find(const Type* object) const srx_noexcept
  {
    return std::find_if(_container.cbegin(),
                        _container.cend(),
                        [object](const TUniquePointer<Type>& element) {
                          return element.get() == object;
                        });
  }

  template<class Type>
  srx_inline typename TObjectContainer<Type>::Iterator
  TObjectContainer<Type>::Find(const RuntimeClass& type) srx_noexcept
  {
    return std::find_if(_container.begin(),
                        _container.end(),
                        [&type](const TUniquePointer<Type>& element) {
                          return element->GetRuntimeClass().IsA(type);
                        });
  }

  template<class Type>
  srx_inline typename TObjectContainer<Type>::ConstIterator
  TObjectContainer<Type>::Find(const RuntimeClass& type) const srx_noexcept
  {
    return std::find_if(_container.cbegin(),
                        _container.cend(),
                        [&type](const TUniquePointer<Type>& item) {
                          return item->GetRuntimeClass().IsA(type);
                        });
  }

  template<class Type>
  template<class T>
  srx_inline typename TObjectContainer<Type>::Iterator
  TObjectContainer<Type>::Find() srx_noexcept
  {
    return Find(GetRuntimeType<T>());
  }
}  // namespace Sorex
