#include <stdio.h>
#include "log.h"

static bool debug_mode = false;

void log_activate(bool active) { debug_mode = active; }

void log_debug(const char *message)
{
    if (!debug_mode)
        return;
    fprintf(stdout, "[*] %s", message);
}

void log_error(const char *message)
{
    if (!debug_mode)
        return;
    fprintf(stdout, "[!] %s", message);
}
