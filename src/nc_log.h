/*
 * twemproxy - A fast and lightweight proxy for memcached protocol.
 * Copyright (C) 2011 Twitter, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _NC_LOG_H_
#define _NC_LOG_H_

#if 1 //shenzheng 2014-12-9 log rotating
#define LOG_ROTATE_DEFAULT			0

#define LOG_FILE_MAX_SIZE_DEFAULT	1073741824
#define LOG_FILE_MAX_SIZE_MIN		1000000
#define LOG_FILE_MAX_SIZE_MAX		1122601371959296

#define LOG_FILE_COUNT_DEFAULT		10
#define LOG_FILE_COUNT_MIN			-1
#define LOG_FILE_COUNT_MAX			LOG_FILES_CIRCULAR_MAX_LEN

/*  LOG_RORATE
  *  0 : twemproxy do not use log rotate functionality.
  *  1 : twemproxy use log rotate functionality.
  */
extern int LOG_RORATE;
extern ssize_t LOG_FIEL_MAX_SIZE_FOR_ROTATING;
#endif

#if 1 //shenzheng 2014-12-15 log rotating step two

/*  LOG_FILE_COUNT_TO_STAY
  *  -1 : do not use log_files_circular, the log bak files do not delete anymore.
  *  =0 : there is no log bak files, only current log file exist.
  *  >0 : use log_files_circular, and only stay newest N log bak files.
  */
extern int		LOG_FILE_COUNT_TO_STAY;
#define LOG_FILES_CIRCULAR_MAX_LEN 200
#endif

struct logger {
    char *name;  /* log file name */
    int  level;  /* log level */
    int  fd;     /* log file descriptor */
    int  nerror; /* # log error */
#if 1 //shenzheng 2014-12-9 log rotating
	ssize_t  current_log_size; /* log current size */
	char*  log_files_circular[LOG_FILES_CIRCULAR_MAX_LEN];	/* we allow stay LOG_FILE_COUNT_TO_STAY log files */
	int circular_cur_pos; /* the current position for the circular */
	bool circular_full; /* the log_files_circular already had LOG_FILE_COUNT_TO_STAY elements*/
	pthread_mutex_t    log_lock;
#endif //shenzheng 2015-7-29 log rotating
};

#define LOG_EMERG   0   /* system in unusable */
#define LOG_ALERT   1   /* action must be taken immediately */
#define LOG_CRIT    2   /* critical conditions */
#define LOG_ERR     3   /* error conditions */
#define LOG_WARN    4   /* warning conditions */
#define LOG_NOTICE  5   /* normal but significant condition (default) */
#define LOG_INFO    6   /* informational */
#define LOG_DEBUG   7   /* debug messages */
#define LOG_VERB    8   /* verbose messages */
#define LOG_VVERB   9   /* verbose messages on crack */
#define LOG_VVVERB  10  /* verbose messages on ganga */
#define LOG_PVERB   11  /* periodic verbose messages on crack */

#define LOG_MAX_LEN 256 /* max length of log message */

/*
 * log_stderr   - log to stderr
 * loga         - log always
 * loga_hexdump - log hexdump always
 * log_error    - error log messages
 * log_warn     - warning log messages
 * log_panic    - log messages followed by a panic
 * ...
 * log_debug    - debug log messages based on a log level
 * log_hexdump  - hexadump -C of a log buffer
 */
#ifdef NC_DEBUG_LOG

#define log_debug(_level, ...) do {                                         \
    if (log_loggable(_level) != 0) {                                        \
        _log(__FILE__, __LINE__, 0, __VA_ARGS__);                           \
    }                                                                       \
} while (0)

#define log_hexdump(_level, _data, _datalen, ...) do {                      \
    if (log_loggable(_level) != 0) {                                        \
        _log(__FILE__, __LINE__, 0, __VA_ARGS__);                           \
        _log_hexdump(__FILE__, __LINE__, (char *)(_data), (int)(_datalen),  \
                     __VA_ARGS__);                                          \
    }                                                                       \
} while (0)

#else

#define log_debug(_level, ...)
#define log_hexdump(_level, _data, _datalen, ...)

#endif

#define log_stderr(...) do {                                                \
    _log_stderr(__VA_ARGS__);                                               \
} while (0)

#define log_safe(...) do {                                                  \
    _log_safe(__VA_ARGS__);                                                 \
} while (0)

#define log_stderr_safe(...) do {                                           \
    _log_stderr_safe(__VA_ARGS__);                                          \
} while (0)

#define loga(...) do {                                                      \
    _log(__FILE__, __LINE__, 0, __VA_ARGS__);                               \
} while (0)

#define loga_hexdump(_data, _datalen, ...) do {                             \
    _log(__FILE__, __LINE__, 0, __VA_ARGS__);                               \
    _log_hexdump(__FILE__, __LINE__, (char *)(_data), (int)(_datalen),      \
                 __VA_ARGS__);                                              \
} while (0)                                                                 \

#define log_error(...) do {                                                 \
    if (log_loggable(LOG_ALERT) != 0) {                                     \
        _log(__FILE__, __LINE__, 0, __VA_ARGS__);                           \
    }                                                                       \
} while (0)

#define log_warn(...) do {                                                  \
    if (log_loggable(LOG_WARN) != 0) {                                      \
        _log(__FILE__, __LINE__, 0, __VA_ARGS__);                           \
    }                                                                       \
} while (0)

#define log_panic(...) do {                                                 \
    if (log_loggable(LOG_EMERG) != 0) {                                     \
        _log(__FILE__, __LINE__, 1, __VA_ARGS__);                           \
    }                                                                       \
} while (0)

int log_init(int level, char *filename);
void log_deinit(void);
void log_level_up(void);
void log_level_down(void);
void log_level_set(int level);
void log_stacktrace(void);
void log_reopen(void);
int log_loggable(int level);
void _log(const char *file, int line, int panic, const char *fmt, ...);
void _log_stderr(const char *fmt, ...);
void _log_safe(const char *fmt, ...);
void _log_stderr_safe(const char *fmt, ...);
void _log_hexdump(const char *file, int line, char *data, int datalen, const char *fmt, ...);

#if 1 //shenzheng 2014-12-9 log rotating
int log_rotate_init(struct logger *l);
int _log_rotate_deinit(struct logger *l);
int _log_rotating(ssize_t write_bytes, struct logger *l);
#endif

#if 1 //shenzheng 2014-12-15 log rotating step two
int _log_files_circular_maintain(const char * file, struct logger *l);
int _is_log_bak_file(const char * logname, const char * filename);
int _log_files_circular_insert(const char * filename, struct logger *l);
int log_files_circular_init(struct logger *l);
int _log_files_circular_deinit(struct logger *l);
void _log_files_circular_info(struct logger *l);
char * set_log_file_max_size(char *arg);
char * set_log_file_count(char *arg);
#endif //shenzheng 2015-1-26 log rotating step two

#if 1 //shenzheng 2015-2-3 common
void log_all(const char *file, int line, size_t data_len, uint8_t *data, const char *fmt, ...);
#endif //shenzheng 2015-2-3 common

#endif
