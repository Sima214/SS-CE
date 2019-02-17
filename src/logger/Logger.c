#include "Logger.h"

#include <Config.h>
#include <Macros.h>
#include <math/MinMax.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string/Strings.h>

/*
 * Settings
 */
#define LOGFILE_BUFLEN 32U
#define LOGFILE_DIR "logs"
#define MESSAGE_BUFLEN 2048U
#define THREAD_BUFLEN 18U
#define TIME_BUFLEN 14U
#define THREAD_ERROR "INV"
#define RED "\x1B[31m"
#define GRN "\x1B[32m"
#define YEL "\x1B[33m"
#define BLU "\x1B[34m"
#define MAG "\x1B[35m"
#define CYN "\x1B[36m"
#define WHT "\x1B[37m"
#define RESET "\x1B[0m"
static const String LEVEL2STRING[] = {StringStatic("ALL"), StringStatic("VERB"),
                                      StringStatic("DEBUG"), StringStatic("INFO"), StringStatic("WARN"),
                                      StringStatic("ERROR"), StringStatic("FATAL"), StringStatic("OFF")};
static const String LEVEL2COLOR[] = {StringStatic(WHT), StringStatic(WHT),
                                     StringStatic(BLU), StringStatic(GRN), StringStatic(CYN),
                                     StringStatic(YEL), StringStatic(RED), StringStatic(RESET)};
/*
 * State variables.
 */
static LogLevel log_level = ALL;
static FILE* log_file = NULL;

static void create_time(String* str) {
  time_t curtime;
  struct tm curtime_table;
  time(&curtime);
  localtime_r(&curtime, &curtime_table);
  size_t size = snprintf(str->array, TIME_BUFLEN, "%02i:%02i:%02i",
                         curtime_table.tm_hour, curtime_table.tm_min, curtime_table.tm_sec);
  str->len = min(size, TIME_BUFLEN);
}
static void output_buffers(LogLevel l, String time, String thread, String msg) {
  //Separators.
  const String SEP0 = StringStatic(" [");
  const String SEP1 = StringStatic("|");
  const String SEP2 = StringStatic("] ");
  const String END = StringStatic(RESET);
  //Create colored output for console.
  String term;
  term = string_concat(9, LEVEL2COLOR[l], time, SEP0, LEVEL2STRING[l],
                      SEP1, thread, SEP2, msg, END);
  if(term.array != NULL) {
    native_puts(term.array);
    free(term.array);
  }
  if(log_file) {
    String file;
    file = string_concat(7, time, SEP0, LEVEL2STRING[l], SEP1, thread, SEP2, msg);
    if(file.array != NULL) {
      fputs(file.array, log_file);
      free(file.array);
    }
  }
}
/*
 * Called by Lifecycle_<os>.c
 */
void setup_log_file() {
  #ifdef MODULE_LOGGER_FILE
    //Get time.
    time_t curtime;
    struct tm curtime_table;
    time(&curtime);
    localtime_r(&curtime, &curtime_table);
    //Make subdirectory.
    ssce_mkdir(LOGFILE_DIR);
    //Open file.
    char name[LOGFILE_BUFLEN];
    snprintf(name, LOGFILE_BUFLEN, "%s/%04i%02i%02i_%02i%02i%02i.log", LOGFILE_DIR, curtime_table.tm_year + 1900,
      curtime_table.tm_mon, curtime_table.tm_mday, curtime_table.tm_hour, curtime_table.tm_min, curtime_table.tm_sec);
    log_file = fopen(name, "w");
  #endif
}
void close_log_file() {
  #ifdef MODULE_LOGGER_FILE
    if(log_file) {
      fclose(log_file);
    }
  #endif
}
/*
 * Api functions.
 */
void ssce_set_log_level(LogLevel l) {
  if(l < ALL || l > OFF) {
    printf("Log level: %i is invalid.\n", l);
    abort();
  }
  else {
    log_level = l;
  }
}
/*
 * Default implementation.
 */
void ssce_log(const LogLevel l, const int o, const char* fmt, ...) {
  //0. Quick exit if the current LogLevel is invalid.
  if(l < log_level) {
    return;
  }
  //1. Allocate stack space.
  char bmsg[MESSAGE_BUFLEN];
  char bthread[THREAD_BUFLEN];
  char btime[TIME_BUFLEN];
  String smsg = StringStatic(bmsg);
  String sthread = StringStatic(bthread);
  String stime = StringStatic(btime);
  //2. Format message.
  {
    va_list vargs;
    va_start(vargs, fmt);
    size_t len = vsnprintf(bmsg, MESSAGE_BUFLEN, fmt, vargs);
    va_end(vargs);
    if(len > 0) {
      smsg.len = min(MESSAGE_BUFLEN - 1, len);
    }
    else {
      //If message formatting failed, then fail quietly.
      return;
    }
  }
  //3. Get thread name.
  if(pthread_getname_np(pthread_self(), bthread, THREAD_BUFLEN) != 0) {
    //Use default id if an error occurs.
    memcpy(bthread, THREAD_ERROR, sizeof(THREAD_ERROR) / sizeof(char));
  }
  sthread.len = strlen(bthread);
  //4. Fill time string.
  create_time(&stime);
  //5. Finalize.
  output_buffers(l, stime, sthread, smsg);
  //6. Extras
  if(MASK_TEST(o, SSCE_LOGGER_ABORT)) {
    abort();
  }
}