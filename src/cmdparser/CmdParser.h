#ifndef SSCE_CMDPARSER_H
#define SSCE_CMDPARSER_H
#include <Macros.h>
#include <stddef.h>
#include <stdint.h>
/*
 * This collects all the non-option arguments.
 */
#define SSCE_DEFAULT_NAME ""
/*
 * Callback function for SSCE_TYPE_CALLBACK.
 * Must return 0 on success.
 */
typedef int (*CmdCallback)(const char* name, const char* arg);
/*
 * Types of argument types that can be read.
 */
typedef enum {
  SSCE_TYPE_NONE=0,
  SSCE_TYPE_STRING,
  SSCE_TYPE_INT32,
  SSCE_TYPE_INT64,
  SSCE_TYPE_FLOAT,
  SSCE_TYPE_DOUBLE,
  SSCE_TYPE_CALLBACK
} OPTION_TYPES;
/*
 * Command line option entries.
 * full_name: required full name of option. '--' gets appended.
 * short_name: optional. '-' gets appended.
 * type: one of OPTION_TYPES.
 * data: pointer to where the data should be stored.
 * description: string to be displayed at the help screen.
 */
typedef struct {
  const char* full_name;
  char short_name;
  OPTION_TYPES type;
  void* data;
  const char* description;
} CmdOption;
/*
 * Parses command line arguments
 * based on the option entries.
 * Parameter 'o' is an array where last
 * element is {NULL}.
 */
extern void parse_cmd_args(const CmdOption* o, int argc, char** argv);
#endif /*SSCE_CMDPARSER_H*/