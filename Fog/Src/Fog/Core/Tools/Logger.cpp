// [Fog-Core]
//
// [License]
// MIT, See COPYING file in package

// [Precompiled Headers]
#if defined(FOG_PRECOMP)
#include FOG_PRECOMP
#endif // FOG_PRECOMP

// [Dependencies]
#include <Fog/Core/Global/Init_p.h>
#include <Fog/Core/Kernel/Application.h>
#include <Fog/Core/Threading/Thread.h>
#include <Fog/Core/Threading/ThreadLocal.h>
#include <Fog/Core/Tools/Logger.h>

namespace Fog {

// ============================================================================
// [Fog::Logger - Global]
// ============================================================================

static Logger* Logger_global;
static Static<ThreadLocal> Logger_tls;

static const char Logger_severityMsg[] = { 'I', 'D', 'W', 'E', 'F', 'A', ' ', ' ' };

static void FOG_CDECL Logger_tls_destructor(void* value)
{
  Logger* logger = static_cast<Logger*>(value);
  
  if (logger != NULL)
    logger->release();
}

// ============================================================================
// [Fog::LoggerRecord - Construction / Destruction]
// ============================================================================

LoggerRecord::LoggerRecord()
{
}

LoggerRecord::LoggerRecord(const Time& time, uintptr_t threadId, uint32_t severity, const StringW& message) :
  _time(time),
  _threadId(threadId),
  _severity(severity),
#if FOG_ARCH_BITS >= 64
  _reserved(0),
#endif // FOG_ARCH_BITS >= 64
  _message(message)
{
}

LoggerRecord::LoggerRecord(const LoggerRecord& other) :
  _time(other._time),
  _threadId(other._threadId),
  _severity(other._severity),
#if FOG_ARCH_BITS >= 64
  _reserved(0),
#endif // FOG_ARCH_BITS >= 64
  _message(other._message)
{
}

LoggerRecord::~LoggerRecord()
{
}

// ============================================================================
// [Fog::LoggerRecord - Equality]
// ============================================================================

bool LoggerRecord::eq(const LoggerRecord& other) const
{
  return _time     == other._time     &&
         _threadId == other._threadId &&
         _severity == other._severity &&
         _message  == other._message  ;
}

// ============================================================================
// [Fog::LoggerRecord - Operator Overload]
// ============================================================================

LoggerRecord& LoggerRecord::operator=(const LoggerRecord& other)
{
  _time = other._time;
  _threadId = other._threadId;
  _severity = other._severity;
  _message = other._message;

  return *this;
}

// ============================================================================
// [Fog::Logger - Construction / Destruction]
// ============================================================================

Logger::Logger()
{
  _reference.init(1);
  _type = LOGGER_TYPE_NONE;
  _severity = LOGGER_SEVERITY_INFO;
}

Logger::~Logger()
{
}

// ============================================================================
// [Fog::Logger - AddRef / Release]
// ============================================================================

Logger* Logger::addRef() const
{
  _reference.inc();
  return const_cast<Logger*>(this);
}

void Logger::release()
{
  if (_reference.deref())
    fog_delete(this);
}

// ============================================================================
// [Fog::Logger - Accessors]
// ============================================================================

void Logger::setSeverity(uint32_t severity)
{
  _severity = severity;
}

// ============================================================================
// [Fog::Logger - Log]
// ============================================================================

static void Logger_buildPrefix(StringW& dst, const char* where, const char* method)
{
  if (where != NULL)
  {
    dst.append(Ascii8(where));
  }

  if (where != NULL && method != NULL)
    dst.append(Ascii8("::"));

  if (method != NULL)
  {
    dst.append(Ascii8(method));
    dst.append(Ascii8("()"));
  }

  if (where != NULL || method != NULL)
    dst.append(Ascii8(" - "));
}

void Logger::logFormat(uint32_t severity, const char* where, const char* method, const char* fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  logVFormat(severity, where, method, fmt, ap);
  va_end(ap);
}

void Logger::logVFormat(uint32_t severity, const char* where, const char* method, const char* fmt, va_list ap)
{
  if (severity < _severity)
    return;

  uintptr_t threadId = Thread::getCurrentThreadId();

  StringW message;
  message.reserve(256);
  Logger_buildPrefix(message, where, method);
  message.appendVFormat(fmt, ap);
  message.squeeze();

  LoggerRecord record(Time::now(), threadId, severity, message);
  emitRecord(record);
}

// ============================================================================
// [Fog::Logger - Emit]
// ============================================================================

void Logger::emitRecord(const LoggerRecord& record)
{
  // Dummy, must be reimplemented.

  // TODO:
  StringA buf;

  buf.append(Logger_severityMsg[record.getSeverity()]);
  buf.append(' ');
  
  // FIXME: Casted to uint64_t, because it's ambiguous. There should be
  // long/ulong version in StringA/StringW to fix this issue.
  buf.appendInt((uint64_t)record.getThreadId(), 
    FormatInt(16, STRING_FORMAT_CAPITALIZE | STRING_FORMAT_ZERO_PAD, 0, sizeof(void*) * 2));
  buf.append(' ');
  TextCodec::local8().encode(buf, record.getMessage());

  size_t bufLength = buf.getLength();
  fwrite(buf.getData(), bufLength, 1, stderr);

  char nl = '\n';
  if (bufLength == 0 || buf.getAt(bufLength - 1) != nl)
    fwrite(&nl, 1, 1, stderr);
  
  fflush(stderr);
}

// ============================================================================
// [Fog::Logger - Statics - Instance]
// ============================================================================

Logger* Logger::getGlobal()
{
  if (fog_application_safe_to_log == 0)
    return NULL;

  return Logger_global;
}

Logger* Logger::getLocal()
{
  if (fog_application_safe_to_log == 0)
    return NULL;

  Logger* logger = static_cast<Logger*>(Logger_tls->get());

  if (logger != NULL)
    return logger;

  return Logger_global;
}

// ============================================================================
// [Fog::Logger - Statics - Logging]
// ============================================================================

void Logger::log(uint32_t severity, const char* where, const char* method, const char* fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  logv(severity, where, method, fmt, ap);
  va_end(ap);
}

void Logger::logv(uint32_t severity, const char* where, const char* method, const char* fmt, va_list ap)
{
  Logger* logger = getLocal();

  if (logger == NULL)
    return;

  logger->logVFormat(severity, where, method, fmt, ap);
}

void Logger::info(const char* where, const char* method, const char* fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  logv(LOGGER_SEVERITY_INFO, where, method, fmt, ap);
  va_end(ap);
}

void Logger::debug(const char* where, const char* method, const char* fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  logv(LOGGER_SEVERITY_DEBUG, where, method, fmt, ap);
  va_end(ap);
}

void Logger::warning(const char* where, const char* method, const char* fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  logv(LOGGER_SEVERITY_WARNING, where, method, fmt, ap);
  va_end(ap);
}

void Logger::error(const char* where, const char* method, const char* fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  logv(LOGGER_SEVERITY_ERROR, where, method, fmt, ap);
  va_end(ap);
}

void Logger::fatal(const char* where, const char* method, const char* fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  logv(LOGGER_SEVERITY_FATAL, where, method, fmt, ap);
  va_end(ap);
}

void Logger::assertion(const char* where, const char* method, const char* fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  logv(LOGGER_SEVERITY_ASSERTION, where, method, fmt, ap);
  va_end(ap);
}

// ============================================================================
// [Fog::StreamLogger - Construction / Destruction]
// ============================================================================

StreamLogger::StreamLogger(const Stream& stream) :
  _stream(stream)
{
  _type = LOGGER_TYPE_STREAM;
}

StreamLogger::~StreamLogger()
{
}

// ============================================================================
// [Fog::StreamLogger - Emit]
// ============================================================================

void StreamLogger::emitRecord(const LoggerRecord& record)
{
}

// ============================================================================
// [Init / Fini]
// ============================================================================

FOG_NO_EXPORT void Logger_init(void)
{
  Logger_tls.init();
  Logger_tls->create(Logger_tls_destructor);

  Logger_global = fog_new Logger();
  fog_application_safe_to_log = 1;
}

FOG_NO_EXPORT void Logger_fini(void)
{
  fog_application_safe_to_log = 0;

  Logger* logger = AtomicCore<Logger*>::setXchg(&Logger_global, (Logger*)NULL);
  Logger_tls.destroy();

  if (logger)
    logger->release();
}

} // Fog namespace
