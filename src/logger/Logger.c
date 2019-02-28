#include "Logger.h"

#include <Config.h>
#include <Macros.h>
#include <math/MinMax.h>
#include <string/Strings.h>

#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Tunables
#define LOGFILE_BUFLEN 64U
#define MESSAGE_BUFLEN 2048U
#define THREAD_BUFLEN 32U
#define TIME_BUFLEN 16U
#define THREAD_ERROR "???"

// Color codes.
#define RED "\x1B[31m"
#define GRN "\x1B[32m"
#define YEL "\x1B[33m"
#define BLU "\x1B[34m"
#define MAG "\x1B[35m"
#define CYN "\x1B[36m"
#define WHT "\x1B[37m"
#define RESET "\x1B[0m"
static const String LEVEL2COLOR[] = {StringStatic(WHT), StringStatic(WHT),
                                     StringStatic(BLU), StringStatic(GRN), StringStatic(CYN),
                                     StringStatic(YEL), StringStatic(RED), StringStatic(RESET)};

// Separators.
static const String SEP0 = StringStatic(" [");
static const String SEP1 = StringStatic("|");
static const String SEP2 = StringStatic("] ");
static const String END = StringStatic(RESET);
static const String LEVEL2STRING[] = {StringStatic("ALL"), StringStatic("TRACE"),
                                      StringStatic("DEBUG"), StringStatic("INFO"), StringStatic("WARN"),
                                      StringStatic("ERROR"), StringStatic("FATAL"), StringStatic("OFF")};

// State storage.
static LogLevel logger_level = LOGGER_ALL;
#ifdef LOGGER_FILE
  static FILE* logger_file = NULL;
#endif

/**
 * Helper functions.
 */
static void create_time(String* str) {
  time_t curtime;
  struct tm curtime_table;
  time(&curtime);
  localtime_r(&curtime, &curtime_table);
  size_t size = snprintf(str->array, TIME_BUFLEN, "%02i:%02i:%02i",
                         curtime_table.tm_hour, curtime_table.tm_min, curtime_table.tm_sec);
  str->len = math_min(size, TIME_BUFLEN);
}

static void output_buffers(LogLevel l, String time, String thread, String msg) {
  //Create colored output for console.
  String term;
  term = string_concat(9, LEVEL2COLOR[l], time, SEP0, LEVEL2STRING[l],
                      SEP1, thread, SEP2, msg, END);
  if(term.array != NULL) {
    native_puts(term.array);
    free(term.array);
  }
  #ifdef LOGGER_FILE
    if(logger_file) {
      String file;
      file = string_concat(7, time, SEP0, LEVEL2STRING[l], SEP1, thread, SEP2, msg);
      if(file.array != NULL) {
        fputs(file.array, logger_file);
        free(file.array);
      }
    }
  #endif
}

/*
 * Internal functions - module lifecycle.
 */
void internal_logger_init() {
  #ifdef LOGGER_FILE
    // Get time.
    time_t curtime;
    struct tm curtime_table;
    time(&curtime);
    localtime_r(&curtime, &curtime_table);
    // Make subdirectories.
    //ssce_mkdir(LOGFILE_DIR);
    // Open file.
    char name[LOGFILE_BUFLEN];
    snprintf(name, LOGFILE_BUFLEN, "%s/%04i%02i%02i_%02i%02i%02i.log", LOGGER_FILE_PREFIX, curtime_table.tm_year + 1900,
      curtime_table.tm_mon, curtime_table.tm_mday, curtime_table.tm_hour, curtime_table.tm_min, curtime_table.tm_sec);
    logger_file = fopen(name, "w");
  #endif
}

void internal_logger_exit() {
  #ifdef LOGGER_FILE
    if(logger_file != NULL) {
      fclose(logger_file);
    }
  #endif
}

/*
 * Api functions.
 */
void logger_set_level(LogLevel l) {
  if(l < LOGGER_ALL || l > LOGGER_OFF) {
    EARLY_TRACE("Invalid argument to logger_set_level!");
    // Fail silently.
  } else {
    logger_level = l;
  }
}

LogLevel logger_get_level() {
  return logger_level;
}

/*
 * Logger implementation.
 */
void logger_log(const LogLevel l, const int o, const char* fmt, ...) {
  // 0. Quick exit if the current LogLevel filters out this message.
  if(HOT_BRANCH(l < logger_level)) {
    return;
  }
  // 1. Allocate stack space.
  char bmsg[MESSAGE_BUFLEN];
  char bthread[THREAD_BUFLEN];
  char btime[TIME_BUFLEN];
  String smsg = StringStatic(bmsg);
  String sthread = StringStatic(bthread);
  String stime = StringStatic(btime);
  // 2. Format message.
  {
    va_list vargs;
    va_start(vargs, fmt);
    size_t len = vsnprintf(bmsg, MESSAGE_BUFLEN, fmt, vargs);
    va_end(vargs);
    if(HOT_BRANCH(len > 0)) {
      smsg.len = math_min(MESSAGE_BUFLEN - 1, len);
    }
    else {
      //If message formatting failed, then fail quietly.
      return;
    }
  }
  // 3. Get thread name.
  if(pthread_getname_np(pthread_self(), bthread, THREAD_BUFLEN) != 0) {
    //Use default id if an error occurs.
    memcpy(bthread, THREAD_ERROR, sizeof(THREAD_ERROR) / sizeof(char));
  }
  sthread.len = strlen(bthread);
  // 4. Fill time string.
  create_time(&stime);
  // 5. Finalize.
  output_buffers(l, stime, sthread, smsg);
  // 6. Extras
  if(MASK_TEST(o, LOGGER_ABORT)) {
    abort();
  }
}