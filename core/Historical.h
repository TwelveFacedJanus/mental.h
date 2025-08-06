#ifndef HISTORICAL_H
#define HISTORICAL_H

#include "Mental.h"

#define LOG_LEVEL_DEBUG 0
#define LOG_LEVEL_INFO  1
#define LOG_LEVEL_WARN  2
#define LOG_LEVEL_ERROR 3
#define LOG_LEVEL_ALL   4

extern int g_log_level;


#define MENTAL_DEBUG(fmt, ...) \
    do { \
        if (g_log_level == LOG_LEVEL_DEBUG || g_log_level == LOG_LEVEL_ALL) { \
            fprintf(stderr, "[%s] DEBUG: %s:%d: " fmt "\n", \
                    get_timestamp(), __FILE__, __LINE__, ##__VA_ARGS__); \
        } \
    } while (0)
    
#define MENTAL_INFO(fmt, ...) \
    do { \
        if (g_log_level == LOG_LEVEL_INFO || g_log_level == LOG_LEVEL_ALL) { \
            fprintf(stderr, "[%s] INFO: " fmt "\n", \
                    get_timestamp(), ##__VA_ARGS__); \
        } \
    } while (0)

#define MENTAL_WARN(fmt, ...) \
    do { \
        if (g_log_level == LOG_LEVEL_WARN || g_log_level == LOG_LEVEL_ALL) { \
            fprintf(stderr, "[%s] WARN: %s:%d: " fmt "\n", \
                    get_timestamp(), __FILE__, __LINE__, ##__VA_ARGS__); \
        } \
    } while (0)

#define MENTAL_ERROR(fmt, ...) \
    do { \
        if (g_log_level == LOG_LEVEL_ERROR || g_log_level == LOG_LEVEL_ALL) { \
            fprintf(stderr, "[%s] ERROR: %s:%d: " fmt "\n", \
                    get_timestamp(), __FILE__, __LINE__, ##__VA_ARGS__); \
        } \
    } while (0)

static inline const char* get_timestamp()
{
    static char timestamp[20];
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);
    return timestamp;
}

#endif // HISTORICAL_H