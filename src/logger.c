#include "logger.h"

#include <stdarg.h>
#include <stdio.h>
#include <time.h>

#define LOG_BUFFER_SIZE  1024
#define TIME_BUFFER_SIZE 32

void logMessage(const char* const tag, const char* const format, ...)
{
    char logBuffer[LOG_BUFFER_SIZE];
    char timeBuffer[TIME_BUFFER_SIZE];

    va_list args;
    va_start(args, format);
    vsnprintf(logBuffer, sizeof(logBuffer), format, args);
    va_end(args);

    time_t currTime;
    time(&currTime);

    strftime(timeBuffer, sizeof(timeBuffer), "%c", localtime(&currTime));

    // I believe fprintf is threadsafe.
    fprintf(stderr, "%s [%s]: %s\n", timeBuffer, tag, logBuffer);
}