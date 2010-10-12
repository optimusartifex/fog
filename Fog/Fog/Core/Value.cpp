// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Assert.h>
#include <Fog/Core/Constants.h>
#include <Fog/Core/Memory.h>
#include <Fog/Core/String.h>
#include <Fog/Core/StringUtil.h>
#include <Fog/Core/Value.h>

#include <errno.h>

#if defined(FOG_OS_WINDOWS)
#include <windows.h>
#endif // FOG_OS_WINDOWS

namespace Fog {

// ============================================================================
// [Fog::ValueData]
// ============================================================================

ValueData::ValueData(uint32_t _type)
{
  refCount.init(1);
  type = _type;
  flags = 0;
  memset(raw, 0, sizeof(raw));
  ptr = NULL;
}

ValueData::~ValueData()
{
}

void* ValueData::allogetData()
{
  return Memory::alloc(sizeof(ValueData));
}

void ValueData::freeData(void* data)
{
  free(data);
}

ValueData* ValueData::ref() const
{
  refCount.inc();
  return const_cast<ValueData*>(this);
}

void ValueData::deref()
{
  if (refCount.deref())
  {
    this->~ValueData();
    freeData(this);
  }
}

// ============================================================================
// [Fog::NullValueData]
// ============================================================================

struct FOG_HIDDEN NullValueData : public ValueData
{
  NullValueData();

  virtual err_t clone(void* dst) const;
  virtual err_t getInt32(int32_t* dst) const;
  virtual err_t getInt64(int64_t* dst) const;
  virtual err_t getDouble(double* dst) const;
  virtual err_t getString(String* dst) const;

  virtual err_t setValue(void* val);
};

NullValueData::NullValueData() : 
  ValueData(VALUE_TYPE_NULL)
{
}

err_t NullValueData::clone(void* dst) const
{
  fog_new_p(dst) NullValueData();
  return ERR_OK;
}

err_t NullValueData::getInt32(int32_t* dst) const
{
  *dst = 0;
  return ERR_OK;
}

err_t NullValueData::getInt64(int64_t* dst) const
{
  *dst = 0;
  return ERR_OK;
}

err_t NullValueData::getDouble(double* dst) const
{
  *dst = 0.0;
  return ERR_OK;
}

err_t NullValueData::getString(String* dst) const
{
  dst->clear();
  return ERR_OK;
}

err_t NullValueData::setValue(void* val)
{
  return ERR_RT_INVALID_OBJECT;
}

// ============================================================================
// [Fog::IntegerValueData]
// ============================================================================

struct FOG_HIDDEN IntegerValueData : public ValueData
{
  IntegerValueData(int64_t content);

  virtual err_t clone(void* dst) const;
  virtual err_t getInt32(int32_t* dst) const;
  virtual err_t getInt64(int64_t* dst) const;
  virtual err_t getDouble(double* dst) const;
  virtual err_t getString(String* dst) const;

  virtual err_t setValue(void* val);
};

IntegerValueData::IntegerValueData(int64_t content) : 
  ValueData(VALUE_TYPE_INTEGER)
{
  i64 = content;
}

err_t IntegerValueData::clone(void* dst) const
{
  fog_new_p(dst) IntegerValueData(i64);
  return ERR_OK;
}

err_t IntegerValueData::getInt32(int32_t* dst) const
{
  if (i64 < INT32_MIN)
  {
    *dst = INT32_MIN;
    return ERR_RT_OVERFLOW;
  }
  else if (i64 > INT32_MAX)
  {
    *dst = INT32_MAX;
    return ERR_RT_OVERFLOW;
  }
  else
  {
    *dst = (int32_t)i64;
    return ERR_OK;
  }
}

err_t IntegerValueData::getInt64(int64_t* dst) const
{
  *dst = i64;
  return ERR_OK;
}

err_t IntegerValueData::getDouble(double* dst) const
{
  *dst = (double)i64;
  return ERR_OK;
}

err_t IntegerValueData::getString(String* dst) const
{
  dst->setInt(i64);
  return ERR_OK;
}

err_t IntegerValueData::setValue(void* val)
{
  i64 = *(int64_t *)val;
  return ERR_OK;
}

// ============================================================================
// [Fog::DoubleValueData]
// ============================================================================

struct FOG_HIDDEN DoubleValueData : public ValueData
{
  DoubleValueData(double content);

  virtual err_t clone(void* dst) const;
  virtual err_t getInt32(int32_t* dst) const;
  virtual err_t getInt64(int64_t* dst) const;
  virtual err_t getDouble(double* dst) const;
  virtual err_t getString(String* dst) const;

  virtual err_t setValue(void* val);
};

DoubleValueData::DoubleValueData(double content) : 
  ValueData(VALUE_TYPE_DOUBLE)
{
  d = content;
}

err_t DoubleValueData::clone(void* dst) const
{
  fog_new_p(dst) DoubleValueData(d);
  return ERR_OK;
}

err_t DoubleValueData::getInt32(int32_t* dst) const
{
  if (d < INT32_MIN)
  {
    *dst = INT32_MIN;
    return ERR_RT_OVERFLOW;
  }
  else if (d > INT32_MAX)
  {
    *dst = INT32_MAX;
    return ERR_RT_OVERFLOW;
  }
  else
  {
    *dst = (int32_t)d;
    return ERR_OK;
  }
}

err_t DoubleValueData::getInt64(int64_t* dst) const
{
  if (d < INT64_MIN)
  {
    *dst = INT64_MIN;
    return ERR_RT_OVERFLOW;
  }
  else if (d > INT64_MAX)
  {
    *dst = INT64_MAX;
    return ERR_RT_OVERFLOW;
  }
  else
  {
    *dst = (int64_t)d;
    return ERR_OK;
  }
}

err_t DoubleValueData::getDouble(double* dst) const
{
  *dst = d;
  return ERR_OK;
}

err_t DoubleValueData::getString(String* dst) const
{
  dst->setDouble(d);
  return ERR_OK;
}

err_t DoubleValueData::setValue(void* val)
{
  d = *(double *)val;
  return ERR_OK;
}

// ============================================================================
// [Fog::StringValueData]
// ============================================================================

struct FOG_HIDDEN StringValueData : public ValueData
{
  StringValueData(const String& content);
  ~StringValueData();

  virtual err_t clone(void* dst) const;
  virtual err_t getInt32(int32_t* dst) const;
  virtual err_t getInt64(int64_t* dst) const;
  virtual err_t getDouble(double* dst) const;
  virtual err_t getString(String* dst) const;

  virtual err_t setValue(void* val);

  FOG_INLINE String* str() const { return (String*)raw; }
};

StringValueData::StringValueData(const String& content) : 
  ValueData(VALUE_TYPE_STRING)
{
  fog_new_p(str()) String(content);
}

StringValueData::~StringValueData()
{
  str()->~String();  
}

err_t StringValueData::clone(void* dst) const
{
  fog_new_p(dst) StringValueData(*str());
  return ERR_OK;
}

err_t StringValueData::getInt32(int32_t* dst) const
{
  return str()->atoi32(dst);
}

err_t StringValueData::getInt64(int64_t* dst) const
{
  return str()->atoi64(dst);
}

err_t StringValueData::getDouble(double* dst) const
{
  return str()->atod(dst);
}

err_t StringValueData::getString(String* dst) const
{
  dst->set(*str());
  return ERR_OK;
}

err_t StringValueData::setValue(void* val)
{
  str()->set(*(String*)val);
  return ERR_OK;
}

// ============================================================================
// [Fog::Value]
// ============================================================================

static Static<NullValueData> _dnullData;
ValueData* Value::_dnull;

Value::Value()
{
  _d = _dnull;
  _d->refCount.inc();
}

Value::Value(const Value& other)
{
  _d = other._d->ref();
}

Value::~Value()
{
  _d->deref();
}

Value& Value::operator=(const Value& other)
{
  atomicPtrXchg(&_d, other._d->ref())->deref();
  return *this;
}

Value Value::null()
{
  return Value();
}

Value Value::fromBool(bool val)
{
  return Value(
    fog_new_p(ValueData::allogetData())
      IntegerValueData(static_cast<int64_t>(val)));
}

Value Value::fromInt32(int32_t val)
{
  return Value(
    fog_new_p(ValueData::allogetData())
      IntegerValueData(static_cast<int64_t>(val)));
}

Value Value::fromInt64(int64_t val)
{
  return Value(
    fog_new_p(ValueData::allogetData())
      IntegerValueData(val));
}

Value Value::fromDouble(double val)
{
  return Value(
    fog_new_p(ValueData::allogetData())
      DoubleValueData(val));
}

Value Value::fromString(const String& val)
{
  return Value(
    fog_new_p(ValueData::allogetData())
      StringValueData(val));
}

Value Value::fromErrno()
{
  return Value(
    fog_new_p(ValueData::allogetData())
      IntegerValueData(errno));
}

#if defined(FOG_OS_WINDOWS)
Value Value::fromWinLastError()
{
  return Value(
    fog_new_p(ValueData::allogetData())
      IntegerValueData(GetLastError()));
}
#endif // FOG_OS_WINDOWS

err_t Value::detach()
{
  if (isDetached()) return ERR_OK;

  ValueData* d = (ValueData*)ValueData::allogetData();
  if (!d) return ERR_RT_OUT_OF_MEMORY;

  _d->clone(d);
  atomicPtrXchg(&_d, d)->deref();
  return ERR_OK;
}

err_t Value::reset()
{
  _dnull->refCount.inc();
  atomicPtrXchg(&_d, _dnull)->deref();
  return ERR_OK;
}

err_t Value::setInt32(int32_t val)
{
  return setInt64(static_cast<int64_t>(val));
}

err_t Value::setInt64(int64_t val)
{
  if (isDetached())
  {
    if (!isInteger())
    {
      _d->~ValueData();
      fog_new_p(_d) IntegerValueData(val);
    }
    else
    {
      _d->i64 = val;
    }
  }
  else
  {
    void* p = ValueData::allogetData();
    if (!p) return ERR_RT_OUT_OF_MEMORY;

    atomicPtrXchg(&_d, 
      reinterpret_cast<ValueData*>(fog_new_p(p) IntegerValueData(val)))->deref();
  }
  return ERR_OK;
}

err_t Value::setDouble(double val)
{
  if (isDetached())
  {
    if (!isDouble())
    {
      _d->~ValueData();
      fog_new_p(_d) DoubleValueData(val);
    }
    else
    {
      _d->d = val;
    }
  }
  else
  {
    void* p = ValueData::allogetData();
    if (!p) return ERR_RT_OUT_OF_MEMORY;

    atomicPtrXchg(&_d,
      reinterpret_cast<ValueData*>(fog_new_p(p) DoubleValueData(val)))->deref();
  }
  return ERR_OK;
}

err_t Value::setString(const String& val)
{
  if (isDetached())
  {
    if (!isString())
    {
      _d->~ValueData();
      fog_new_p(_d) StringValueData(val);
    }
    else
    {
      reinterpret_cast<StringValueData*>(_d)->str()->set(val);
    }
  }
  else
  {
    void* p = ValueData::allogetData();
    if (!p) return ERR_RT_OUT_OF_MEMORY;

    atomicPtrXchg(&_d,
      reinterpret_cast<ValueData*>(fog_new_p(p) StringValueData(val)))->deref();
  }
  return ERR_OK;
}

} // Fog namespace

// ============================================================================
// [Library Initializers]
// ============================================================================

FOG_INIT_DECLARE err_t fog_value_init(void)
{
  using namespace Fog;

  _dnullData.init();
  Value::_dnull = _dnullData.instancep();

  return ERR_OK;
}

FOG_INIT_DECLARE void fog_value_shutdown(void)
{
  using namespace Fog;

  _dnullData.destroy();
}
