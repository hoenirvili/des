#pragma once

#define log_error(fmt, ...) fprintf (stderr, "[!] " fmt "\n", ##__VA_ARGS__)

#ifdef NDEBUG
#define log_debug(fmt, ...)
#else
#define log_debug(fmt, ...) fprintf(stdout, "[*] " fmt "\n", ##__VA_ARGS__)
#endif
