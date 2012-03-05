// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Guard]
#ifndef _FOG_CORE_TOOLS_LOGGER_H
#define _FOG_CORE_TOOLS_LOGGER_H

// [Dependencies]
#include <Fog/Core/Global/Global.h>
#include <Fog/Core/Threading/Atomic.h>
#include <Fog/Core/Tools/Char.h>
#include <Fog/Core/Tools/Stream.h>
#include <Fog/Core/Tools/String.h>
#include <Fog/Core/Tools/Time.h>

namespace Fog {

//! @addtogroup Fog_Core_Tools
//! @{

// ============================================================================
// [Fog::LoggerRecord]
// ============================================================================

struct FOG_API LoggerRecord
{
  LoggerRecord();
  LoggerRecord(const Time& time, uintptr_t threadId, uint32_t severity, const StringW& message);
  LoggerRecord(const LoggerRecord& other);
  ~LoggerRecord();

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE const Time& getTime() const { return _time; } 
  FOG_INLINE uintptr_t getThreadId() const { return _threadId; }
  FOG_INLINE uint32_t getSeverity() const { return _severity; }
  FOG_INLINE const StringW& getMessage() const { return _message; }

  // --------------------------------------------------------------------------
  // [Equality]
  // --------------------------------------------------------------------------

  bool eq(const LoggerRecord& other) const;

  // --------------------------------------------------------------------------
  // [Operator Overload]
  // --------------------------------------------------------------------------

  LoggerRecord& operator=(const LoggerRecord& other);

  FOG_INLINE bool operator==(const LoggerRecord& other) const { return  eq(other); }
  FOG_INLINE bool operator!=(const LoggerRecord& other) const { return !eq(other); }

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Time _time;
  uintptr_t _threadId;

  uint32_t _severity;
#if FOG_ARCH_BITS >= 64
  uint32_t _reserved;
#endif // FOG_ARCH_BITS
  StringW _message;
};

// ============================================================================
// [Fog::Logger]
// ============================================================================

struct FOG_API Logger
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  Logger();
  virtual ~Logger();

  // --------------------------------------------------------------------------
  // [AddRef / Release]
  // --------------------------------------------------------------------------

  virtual Logger* addRef() const;
  virtual void release();

  // --------------------------------------------------------------------------
  // [Accessors]
  // --------------------------------------------------------------------------

  FOG_INLINE size_t getReference() const { return _reference.get(); }

  FOG_INLINE uint32_t getType() const { return _type; }
  FOG_INLINE uint32_t getSeverity() const { return _severity; }

  virtual void setSeverity(uint32_t severity);

  // --------------------------------------------------------------------------
  // [Log]
  // --------------------------------------------------------------------------

  void logFormat(uint32_t severity, const char* where, const char* method, const char* fmt, ...);
  void logVFormat(uint32_t severity, const char* where, const char* method, const char* fmt, va_list ap);

  // --------------------------------------------------------------------------
  // [Emit]
  // --------------------------------------------------------------------------

  virtual void emitRecord(const LoggerRecord& record);

  // --------------------------------------------------------------------------
  // [Statics - Instance]
  // --------------------------------------------------------------------------

  //! @brief Get the global application logger.
  static Logger* getGlobal();
  //! @brief Get a thread local logger if exists, otherwise the global logger.
  static Logger* getLocal();

  // --------------------------------------------------------------------------
  // [Statics - Logging]
  // --------------------------------------------------------------------------

  //! @brief Static method which should be used to log a message with custom 
  //! @a severity.
  static void log(uint32_t severity, const char* where, const char* method, const char* fmt, ...);
  //! @brief Similar to @ref log, but accepts @c va_list.
  static void logv(uint32_t severity, const char* where, const char* method, const char* fmt, va_list ap);

  //! @brief Static method which should be used to log an informative message.
  static void info(const char* where, const char* method, const char* fmt, ...);
  //! @brief Static method which should be used to log a debug message.
  static void debug(const char* where, const char* method, const char* fmt, ...);
  //! @brief Static method which should be used to log a warning message.
  static void warning(const char* where, const char* method, const char* fmt, ...);
  //! @brief Static method which should be used to log an error message.
  static void error(const char* where, const char* method, const char* fmt, ...);
  //! @brief Static method which should be used to log a fatal message.
  static void fatal(const char* where, const char* method, const char* fmt, ...);
  //! @brief Static method which should be used to log an assertion failure.
  static void assertion(const char* where, const char* method, const char* fmt, ...);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  //! @brief Reference count.
  mutable Atomic<size_t> _reference;

  //! @brief Logger type (see @ref LOGGER_TYPE).
  uint32_t _type;
  //! @brief Severity (see @ref LOGGER_SEVERITY).
  uint32_t _severity;

private:
  FOG_NO_COPY(Logger)
};

// ============================================================================
// [Fog::StreamLogger]
// ============================================================================

struct FOG_API StreamLogger : public Logger
{
  // --------------------------------------------------------------------------
  // [Construction / Destruction]
  // --------------------------------------------------------------------------

  StreamLogger(const Stream& stream);
  virtual ~StreamLogger();

  // --------------------------------------------------------------------------
  // [Emit]
  // --------------------------------------------------------------------------

  virtual void emitRecord(const LoggerRecord& record);

  // --------------------------------------------------------------------------
  // [Members]
  // --------------------------------------------------------------------------

  Stream _stream;

private:
  FOG_NO_COPY(StreamLogger)
};

//! @}

} // Fog namespace

// [Guard]
#endif // _FOG_CORE_TOOLS_LOGGER_H
