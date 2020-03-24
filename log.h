#include <stdbool.h>

/**
 * log_activate
 *
 * activate  - set to true to make all log_* function log to stdout
 */
extern void log_activate(bool activate);

/**
 * log_debug
 *
 * Write messages to stdout
 */
extern void log_debug(const char *message, ...);

/**
 * log_error
 *
 * write messages to stderr
 */
extern void log_error(const char *message, ...);
