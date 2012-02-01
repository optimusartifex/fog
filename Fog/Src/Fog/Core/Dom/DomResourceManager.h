// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_DOM_DOMRESOURCEMANAGER_H
#define _FOG_CORE_DOM_DOMRESOURCEMANAGER_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Tools/Hash.h>
#include <Fog/Core/Tools/List.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/Var.h>

namespace Fog {

//! @addtogroup Fog_Dom
//! @{

// ============================================================================
// [Fog::DomResourceItem]
// ============================================================================

struct FOG_API DomResourceItem
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  DomResourceItem(DomResourceManager* manager, const StringW& name);
  virtual ~DomResourceItem();

  // --------------------------------------------------------------------------
  // [Release]
  // --------------------------------------------------------------------------

  //! @brief Make reference of this resource item (increase reference count).
  virtual DomResourceItem* addRef();

  //! @brief Called when the object no londer need the resource (decrease 
  //! reference count and free if it gets zero).
  virtual void release();

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE DomResourceManager* getManager() const { return _manager; }
  FOG_INLINE size_t getReference() const { return _reference.get(); }

  FOG_INLINE uint32_t getResourceFlags() const { return _resourceFlags; }

  FOG_INLINE err_t getError() const { return _error; }
  FOG_INLINE void setError(err_t error) { _error = error; }

  FOG_INLINE const StringW& getName() const { return _name; }

  FOG_INLINE bool isLoaded() const { return (_resourceFlags & DOM_RESOURCE_FLAG_LOADED) != 0; }
  FOG_INLINE bool isError() const { return (_resourceFlags & DOM_RESOURCE_FLAG_ERROR) != 0; }

  FOG_INLINE const Var& getData() const { return _data; }
  void setData(const Var& data);

  // --------------------------------------------------------------------------
  // [Reset]
  // --------------------------------------------------------------------------

  void reset();

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Atomic<size_t> _reference;
  uint32_t _resourceFlags;
  err_t _error;

  DomResourceManager* _manager;
  StringW _name;
  Var _data;

private:
  _FOG_NO_COPY(DomResourceItem)
};

// ============================================================================
// [Fog::DomResourceManager]
// ============================================================================

struct FOG_API DomResourceManager
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  DomResourceManager();
  ~DomResourceManager();

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const StringW& getResourceRoot() { return _resourceRoot; }
  FOG_INLINE void setResourceRoot(const StringW& s) { _resourceRoot = s; }

  // --------------------------------------------------------------------------
  // [Management]
  // --------------------------------------------------------------------------

  //! @brief Get length of queue (count of pending items).
  size_t getQueueLength() const;

  //! @brief Create external resource item (or reference it if it already exists).
  DomResourceItem* createExternalResource(const StringW& name);

  //! @brief Create internal resource item.
  DomResourceItem* createInternalResource(const Var& data);

  //! @brief Release resource item, called by DomResourceItem::release().
  void _releaseResource(DomResourceItem* item);

  // --------------------------------------------------------------------------
  // [Loader]
  // --------------------------------------------------------------------------

  //! @brief Load all queued items.
  bool loadQueuedResources();

  //! @brief Load a single resource item, called internally.
  bool _loadResource(DomResourceItem* item);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Resource root, currently it's a file-path.
  StringW _resourceRoot;

  //! @brief External resources (map resource name to resource item).
  Hash<StringW, DomResourceItem*> _externalResources;
  //! @brief Internal resources ().
  List<DomResourceItem*> _internalResources;
  //! @brief Resource queue (resources queued to load).
  List<DomResourceItem*> _queue;

private:
  _FOG_NO_COPY(DomResourceManager)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_DOM_DOMRESOURCEMANAGER_H
