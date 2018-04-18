#include "CmdParser.h"

#include <errno.h>
#include <Macros.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static const CmdOption HELP_DUMMY = {"help", 'h', SSCE_TYPE_NONE, NULL, NULL};

static size_t get_option_array_len(const CmdOption* o) {
  size_t r = 0;
  CmdOption c;
  while((c=*o).full_name != NULL){
    r++;
    o++;
  }
  return r;
}

static void handle_help(size_t olen, const CmdOption* o) {
  //TODO: help
}

static const CmdOption* findopt_from_fname(size_t olen, const CmdOption* o, char* name) {
  if(strequal(name, "help")) {
    handle_help(olen, o);
    return &HELP_DUMMY;
  }
  for(size_t i=0; i<olen; i++) {
    if(strequal((o[i]).full_name, name)) {
      return o + i;
    }
  }
  return NULL;
}

static const CmdOption* findopt_from_sname(size_t olen, const CmdOption* o, char name) {
  if(name == 'h') {
    handle_help(olen, o);
    return &HELP_DUMMY;
  }
  for(size_t i=0; i<olen; i++) {
    if((o[i]).short_name == name) {
      return o + i;
    }
  }
  return NULL;
}

static char* clone_string(const char* s) {
  size_t l = strlen(s);
  char* cloned = malloc(l + 1);
  if(cloned == NULL) {
    fprintf(stderr, "Could not allocate memory.");
    abort();
  }
  memcpy(cloned, s, l);
  cloned[l] = '\0';
  return cloned;
}

static void handle_argument(const CmdOption* o, const char* s) {
  switch(o->type){
    case SSCE_TYPE_STRING: *((char**)o->data) = clone_string(s);
      break;
    case SSCE_TYPE_INT32: {
      char* leftovers;
      errno = 0;
      int32_t tmp = strtol(s, &leftovers, 0);
      if(*leftovers != '\0') {
        //Not an integer.
        fprintf(stderr, "Invalid value: --%s expects one integer argument.\nGot: %s", o->full_name, s);
        exit(1);
      }
      else if(errno == ERANGE) {
        //Out of range.
        fprintf(stderr, "Integer argument %s is out of range.", s);
        exit(1);
      }
      else {
        //Success
        *((int32_t*)o->data) = tmp;
      }
      }
      break;
    case SSCE_TYPE_INT64: *((int64_t*)o->data) = strtoll(s, NULL, 0);
      break;
    case SSCE_TYPE_FLOAT: *((float*)o->data) = atof(s);
      break;
    case SSCE_TYPE_DOUBLE: *((double*)o->data) = atof(s);
      break;
    default: {
      fprintf(stderr, "Unhandled enum: %i\nWrong library usage.\n", o->type);
      abort();
    }
  }
}

static void handle_callback(int argc, char* argv[], int* index, const CmdOption* o) {
  CmdCallback callp = (CmdCallback) o->data;
  int arg_offset = *index + 1;
  if(arg_offset < argc) {
    char** sub_argv = &argv[arg_offset];
    int read = callp(o->full_name, argc-arg_offset, sub_argv);
    if(read < 0) {
      fprintf(stderr, "Callback function for option --%s failed.\n", o->full_name);
      abort();
    }
    else {
      *index += read;
    }
  }
  else {
    //No arguments call.
    if(callp(o->full_name, 0, NULL) < 0){
      fprintf(stderr, "Callback function for option --%s failed.\n", o->full_name);
      abort();
    }
  }
}

void ssce_parse_cmdln(const CmdOption* o, int argc, char* argv[]) {
  size_t olen = get_option_array_len(o);
  for(int i=1; i<argc; i++) {
    char* arg = argv[i];
    if(arg[0] == '-') {
      const CmdOption* f;
      if(arg[1] == '-') {
        //Full option.
        f = findopt_from_fname(olen, o, arg+2);
      }
      else {
        //Half option.
        f = findopt_from_sname(olen, o, arg[1]);
      }
      if(f != NULL){
        if(f->type == SSCE_TYPE_CALLBACK) {
          handle_callback(argc, argv, &i, f);
        }
        else if(f->type!=SSCE_TYPE_NONE && f->data!=NULL) {
          //Check if there is an argument after the option.
          //If there is, collect it and handle it.
          if((++i) < argc) {
            handle_argument(f, argv[i]);
          }
          else {
            //Not enough arguments.
            fprintf(stderr, "Not enough arguments: Option %s requires one argument.\nUse --help (or -h) for help.", arg);
          }
        }
      }
      else {
        fprintf(stderr, "Invalid option: %s\nUse --help (or -h) for help.", arg);
        exit(EXIT_FAILURE);
      }
    }
    else {
      const CmdOption* def = findopt_from_fname(olen, o, SSCE_DEFAULT_NAME);
      if(def != NULL) {
        if(def->type == SSCE_TYPE_CALLBACK) {
          handle_callback(argc, argv, &i, def);
        }
        else if(def->type!=SSCE_TYPE_NONE && def->data!=NULL) {
          handle_argument(def, arg);
        }
      }
      else {
        fprintf(stderr, "This program does not accept default arguments.\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}