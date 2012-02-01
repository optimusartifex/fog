// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Kernel/CoreObj.h>

namespace Fog {

// ============================================================================
// [Fog::CoreObj - Construction / Destruction]
// ============================================================================

CoreObj::CoreObj()
{
}

CoreObj::~CoreObj()
{
}

// ============================================================================
// [Fog::CoreObj - Properties - Interface]
// ============================================================================

size_t CoreObj::_getPropertyIndex(const InternedStringW& name) const
{
  return INVALID_INDEX;
}

size_t CoreObj::_getPropertyIndex(const CharW* name, size_t length) const
{
  return INVALID_INDEX;
}

err_t CoreObj::_getPropertyInfo(size_t index, PropertyInfo& info) const
{
  return ERR_OBJ_PROPERTY_NOT_FOUND;
}

err_t CoreObj::_getProperty(size_t index, StringW& value) const
{
  return ERR_OBJ_PROPERTY_NOT_FOUND;
}

err_t CoreObj::_setProperty(size_t index, const StringW& value)
{
  return ERR_OBJ_PROPERTY_NOT_FOUND;
}

err_t CoreObj::_resetProperty(size_t index)
{
  return ERR_OBJ_PROPERTY_NOT_FOUND;
}

err_t CoreObj::_unresolvedProperty(size_t& newIndex,
  const CharW* name, size_t nameLength, const InternedStringW* nameInterned,
  const StringW* initialValue)
{
  // We just don't allow to create dynamic properties by default. So returning
  // 'ERR_OBJ_PROPERTY_NOT_FOUND' is basically the correct way to tell called
  // that the property doesn't exist (and cannot be created).
  return ERR_OBJ_PROPERTY_NOT_FOUND;
}

// ============================================================================
// [Fog::CoreObj - Properties - Public]
// ============================================================================

bool CoreObj::hasProperty(const StringW& name) const
{
  size_t index;
  if (name.isInterned())
    index = _getPropertyIndex(reinterpret_cast<const InternedStringW&>(name));
  else
    index = _getPropertyIndex(name.getData(), name.getLength());
  return index != INVALID_INDEX;
}

bool CoreObj::hasProperty(const StubW& name) const
{
  size_t index = _getPropertyIndex(name.getData(), name.getComputedLength());
  return index != INVALID_INDEX;
}

err_t CoreObj::getProperty(const StringW& name, StringW& value) const
{
  size_t index;
  if (name.isInterned())
    index = getPropertyIndex(reinterpret_cast<const InternedStringW&>(name));
  else
    index = getPropertyIndex(name.getData(), name.getLength());
  return _getProperty(index, value);
}

err_t CoreObj::getProperty(const StubW& name, StringW& value) const
{
  const CharW* nameData = name.getData();
  size_t nameLength = name.getComputedLength();

  size_t index = getPropertyIndex(nameData, nameLength);
  return _getProperty(index, value);
}

err_t CoreObj::setProperty(const StringW& name, const StringW& value)
{
  if (name.isInterned())
  {
    size_t index = getPropertyIndex(reinterpret_cast<const InternedStringW&>(name));
    if (FOG_LIKELY(index != INVALID_INDEX))
      return _setProperty(index, value);
    else
      return _unresolvedProperty(index,
        name.getData(), name.getLength(), reinterpret_cast<const InternedStringW*>(&name),
        &value);
  }
  else
  {
    size_t index = getPropertyIndex(name.getData(), name.getLength());
    if (FOG_LIKELY(index != INVALID_INDEX))
      return _setProperty(index, value);
    else
      return _unresolvedProperty(index,
        name.getData(), name.getLength(), NULL,
        &value);
  }
}

err_t CoreObj::setProperty(const StubW& name, const StringW& value)
{
  const CharW* nameData = name.getData();
  size_t nameLength = name.getComputedLength();

  size_t index = getPropertyIndex(nameData, nameLength);
  if (FOG_LIKELY(index != INVALID_INDEX))
    return _setProperty(index, value);
  else
    return _unresolvedProperty(index, nameData, nameLength, NULL, &value);
}

err_t CoreObj::resetProperty(const StringW& name)
{
  if (name.isInterned())
  {
    size_t index = getPropertyIndex(reinterpret_cast<const InternedStringW&>(name));
    if (FOG_LIKELY(index != INVALID_INDEX))
      return _resetProperty(index);
    else
      return ERR_OBJ_PROPERTY_NOT_FOUND;
  }
  else
  {
    size_t index = getPropertyIndex(name.getData(), name.getLength());
    if (FOG_LIKELY(index != INVALID_INDEX))
      return _resetProperty(index);
    else
      return ERR_OBJ_PROPERTY_NOT_FOUND;
  }
}

err_t CoreObj::resetProperty(const StubW& name)
{
  size_t index = getPropertyIndex(name.getData(), name.getComputedLength());
  if (FOG_LIKELY(index != INVALID_INDEX))
    return _resetProperty(index);
  else
    return ERR_OBJ_PROPERTY_NOT_FOUND;
}

// ============================================================================
// [Init / Fini]
// ============================================================================

} // Fog namespace
