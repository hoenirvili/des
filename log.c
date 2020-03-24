#include <stdio.h>
#include <stdarg.h>

#include "log.h"

static bool debug_mode = false;

void log_activate(bool active) { debug_mode = active; }

static const char *prefix_err = "[!]";

static const char *prefix_debug = "[*]";

void log_debug(const char *fmt, ...)
{
    if (!debug_mode)
        return;

    va_list(args);
    fprintf(stdout, "%s ", prefix_debug);
    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    va_end(args);
}

void log_error(const char *fmt, ...)
{
    va_list(args);
    fprintf(stdout, "%s ", prefix_err);
    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    va_end(args);
}
