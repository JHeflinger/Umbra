#ifndef CONSOLE_H
#define CONSOLE_H

#define CONSOLE_HISTORY 100
#define MAX_LOG_LENGTH 512

typedef enum {
    CONSOLE_TRACE,
    CONSOLE_INFO,
    CONSOLE_WARN,
    CONSOLE_ERROR
} ConsoleLogLevel;

void ConsoleTrace(const char* log);

void ConsoleInfo(const char* log);

void ConsoleWarn(const char* log);

void ConsoleError(const char* log);

void DrawConsole(float x, float y, float w, float h);

#endif