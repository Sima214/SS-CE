#ifndef SSCE_CMDPARSER_H
#define SSCE_CMDPARSER_H
#include <Macros.h>
#include <stddef.h>
#include <stdint.h>
/*
 * This collects all the arguments which do not belong to an option.
 */
#define SSCE_DEFAULT_NAME ""
/*
 * Types of argument types that can be read.
 */
typedef enum {
  SSCE_TYPE_STRING,
  SSCE_TYPE_INT,
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
 * max_count: 1 if data is not an array. Else it is the size of the array in elements.
 * data: pointer to where the data should be stored.
 * description: string to be displayed at the help screen.
 */
typedef struct {
  const char* full_name;
  char short_name;
  int32_t type;
  size_t max_count;
  void* data;
  const char* description;
} CmdOption;
/*
 * Parses command line arguments
 * based on the option entries.
 */
extern void parse_cmd_args(CmdOption*, int argc, char** argv);
#endif /*SSCE_CMDPARSER_H*/