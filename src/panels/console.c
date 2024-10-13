#include "console.h"
#include "raylib.h"

char g_console_history[CONSOLE_HISTORY][MAX_LOG_LENGTH] = { 0 };
ConsoleLogLevel g_console_history_types[CONSOLE_HISTORY] = { 0 };
size_t g_console_num_logs = 0;

void add_console_log(const char* log, ConsoleLogLevel loglevel) {
    if (g_console_num_logs < CONSOLE_HISTORY) {
        strncpy(g_console_history[g_console_num_logs], log, MAX_LOG_LENGTH);
        g_console_history_types[g_console_num_logs] = loglevel;
        g_console_num_logs++;
    } else {
        for (int i = 0; i < CONSOLE_HISTORY - 1; i++) {
            memcpy(g_console_history[i], g_console_history[i + 1], MAX_LOG_LENGTH);
            g_console_history_types[i] = g_console_history_types[i + 1];
        }
        strncpy(g_console_history[g_console_num_logs], log, MAX_LOG_LENGTH);
        g_console_history_types[g_console_num_logs] = loglevel;
    }
}

void ConsoleTrace(const char* log) {
    add_console_log(log, CONSOLE_TRACE);
}

void ConsoleInfo(const char* log) {
    add_console_log(log, CONSOLE_INFO);
}

void ConsoleWarn(const char* log) {
    add_console_log(log, CONSOLE_WARN);
}

void ConsoleError(const char* log) {
    add_console_log(log, CONSOLE_ERROR);
}

void DrawConsole(float x, float y, float w, float h) {
    for (int i = 0; i < g_console_num_logs; i++) {
        int ypos = y + (i * 20);
        if (g_console_history[i][0] != '\0') {
            Color color = g_console_history_types[i] == CONSOLE_TRACE ? RAYWHITE :
                (g_console_history_types[i] == CONSOLE_INFO ? GREEN :
                (g_console_history_types[i] == CONSOLE_WARN ? YELLOW :
                (g_console_history_types[i] == CONSOLE_ERROR ? RED : PURPLE)));
			DrawText(g_console_history[i], x + 5, ypos, 14, color);
        }
    }
}