#ifndef _POLAR_LOGGER_
#define _POLAR_LOGGER_

/// Logs data when given a specific tag:
void logMessage(const char* tag, const char* format, ...);

#ifndef DISABLE_LOG_INFO
#define LOG_INFO(...) logMessage("INFO", __VA_ARGS__)
#else
#define LOG_INFO
#endif

#ifndef DISABLE_LOG_ERROR
#define LOG_ERROR(...) logMessage("ERROR", __VA_ARGS__)
#else
#define LOG_ERROR
#endif

#ifndef DISABLE_LOG_WARN
#define LOG_WARN(...) logMessage("WARN", __VA_ARGS__)
#else
#define LOG_WARN
#endif

#ifndef DISABLE_LOG_DEBUG
#define LOG_DEBUG(...) logMessage("DEBUG", __VA_ARGS__)
#else
#define LOG_DEBUG
#endif

#endif // _POLAR_LOGGER_