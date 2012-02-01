// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Dependencies]
#include <Fog/Core/Dom/DomResourceManager.h>
#include <Fog/Core/OS/FilePath.h>
#include <Fog/Core/Tools/Logger.h>
#include <Fog/Core/Tools/Stream.h>
#include <Fog/G2d/Imaging/Image.h>

namespace Fog {

// ============================================================================
// [Fog::DomResourceManager - Construction / Destruction]
// ============================================================================

DomResourceManager::DomResourceManager()
{
}

DomResourceManager::~DomResourceManager()
{
}

// ============================================================================
// [Fog::DomResourceManager - Accessors]
// ============================================================================

size_t DomResourceManager::getQueueLength() const
{
  return _queue.getLength();
}

// ============================================================================
// [Fog::DomResourceManager - Management]
// ============================================================================

DomResourceItem* DomResourceManager::createExternalResource(const StringW& name)
{
  DomResourceItem* item = _externalResources.get(name, NULL);
  err_t err;
  
  if (item != NULL)
    return item->addRef();

  item = fog_new DomResourceItem(this, name);
  if (FOG_IS_NULL(item))
  {
    Logger::error("Fog::DomResourceManager", "createExternalResource",
      "Failed to allocate memory for DomResourceItem.");
    return NULL;
  }

  err = _externalResources.put(name, item);
  if (FOG_IS_ERROR(err))
  {
    Logger::error("Fog::DomResourceManager", "createExternalResource",
      "Failed to add resource item into the _externalResources map.");

    fog_delete(item);
    return NULL;
  }

  err = _queue.append(item);
  if (FOG_IS_ERROR(err))
  {
    Logger::error("Fog::DomResourceManager", "createExternalResource",
      "Failed to add resource to the queue.");
    _externalResources.remove(name);

    fog_delete(item);
    return NULL;
  }

  return item;
}

DomResourceItem* DomResourceManager::createInternalResource(const Var& data)
{
  DomResourceItem* item = fog_new DomResourceItem(this, StringW::getEmptyInstance());
  
  if (FOG_IS_NULL(item))
  {
    Logger::error("Fog::DomResourceManager", "createInternalResource",
      "Failed to allocate memory for DomResourceItem.");
    return NULL;
  }
  
  err_t err = _internalResources.append(item);
  if (FOG_IS_ERROR(err))
  {
    Logger::error("Fog::DomResourceManager", "createInternalResource",
      "Failed to add DomResourceItem into the _internalResources list.");

    fog_delete(item);
    return NULL;
  }

  item->_resourceFlags |= DOM_RESOURCE_FLAG_LOADED;
  item->_data = data;

  return item;
}

void DomResourceManager::_releaseResource(DomResourceItem* item)
{
  size_t i = _queue.indexOf(item);
  if (i != INVALID_INDEX)
    _queue.removeAt(i);

  if (item->_name.isEmpty())
  {
    i = _internalResources.indexOf(item);
    FOG_ASSERT(i != INVALID_INDEX);

    if (_internalResources.removeAt(i) != ERR_OK)
    {
      Logger::error("Fog::DomResourceManager", "_releaseResourceItem",
        "Failed to remove resource item from _internalResources list.");
    }
  }
  else
  {
    if (_externalResources.remove(item->_name) != ERR_OK)
    {
      Logger::error("Fog::DomResourceManager", "_releaseResourceItem",
        "Failed to remove resource item from _externalResource map.");
    }
  }

  fog_delete(item);
}

// ============================================================================
// [Fog::DomResourceManager - Loader]
// ============================================================================

bool DomResourceManager::loadQueuedResources()
{
  size_t i = 0;
  
  while (_queue.getLength() > i)
  {
    DomResourceItem* item =_queue.getAt(i);

    if (!_loadResource(item))
      i++;
    else
      _queue.removeAt(i);
  }

  return _queue.isEmpty();
}

bool DomResourceManager::_loadResource(DomResourceItem* item)
{
  err_t err;

  StringW resourceName = item->getName();
  StringW extension;

  err = FilePath::extractExtension(extension, resourceName);
  if (FOG_IS_ERROR(err))
  {
    Logger::error("Fog::DomResourceManager", "_loadResourceItem",
      "Failed to extract resource extension.");

    item->setError(err);
    return false;
  }

  // We use just lowercased extension for simplicity.
  extension.lower();

  // --------------------------------------------------------------------------
  // [Image Resource]
  // --------------------------------------------------------------------------

  // We accept only BMP, JPEG, and PNG as image resources.
  if (extension == Ascii8("bmp" ) ||
      extension == Ascii8("jpg" ) ||
      extension == Ascii8("jpeg") ||
      extension == Ascii8("png" ) )
  {
    StringW fullPath;
    err = FilePath::join(fullPath, getResourceRoot(), resourceName);

    if (FOG_IS_ERROR(err))
    {
      Logger::error("Fog::DomResourceManager", "_loadResourceItem",
        "Failed to join the resource root and the resource path.");

      item->setError(err);
      return false;
    }

    Stream stream;
    err = stream.openFile(fullPath, STREAM_OPEN_READ);

    if (FOG_IS_ERROR(err))
    {
      Logger::error("Fog::DomResourceManager", "_loadResourceItem",
        "Failed to open the resource stream.");

      item->setError(err);
      return false;
    }

    Image image;
    err = image.readFromStream(stream);

    if (FOG_IS_ERROR(err))
    {
      Logger::error("Fog::DomResourceManager", "_loadResourceItem",
        "Failed to load the image resource.");

      item->setError(err);
      return false;
    }

    item->setData(Var::fromImage(image));
    return true;
  }

  // --------------------------------------------------------------------------
  // [Unknown]
  // --------------------------------------------------------------------------

  Logger::error("Fog::DomResourceManager", "_loadResourceItem",
    "Failed to recognize the resource type.");

  item->setError(ERR_RT_INVALID_STATE);
  return false;
}

// ============================================================================
// [Fog::DomResourceItem - Construction / Destruction]
// ============================================================================

DomResourceItem::DomResourceItem(DomResourceManager* manager, const StringW& name) :
  _resourceFlags(NO_FLAGS),
  _error(ERR_OK),
  _manager(manager),
  _name(name)
{
  _reference.init(1);
}

DomResourceItem::~DomResourceItem()
{
}

// ============================================================================
// [Fog::DomResourceItem - Release]
// ============================================================================

DomResourceItem* DomResourceItem::addRef()
{
  _reference.inc();
  return this;
}

void DomResourceItem::release()
{
  if (_reference.deref())
    _manager->_releaseResource(this);
}

// ============================================================================
// [Fog::DomResourceItem - Resource]
// ============================================================================

void DomResourceItem::reset()
{
  _resourceFlags &= ~(DOM_RESOURCE_FLAG_LOADED | DOM_RESOURCE_FLAG_ERROR);
  _error = ERR_OK;

  _name.reset();
  _data.reset();
}

void DomResourceItem::setData(const Var& data)
{
  _resourceFlags |= DOM_RESOURCE_FLAG_LOADED;
  _data = data;
}

} // Fog namespace
