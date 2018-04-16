#include "CmdParser.h"

#include <Macros.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static const CmdOption HELP_DUMMY = {"help", 'h', SSCE_TYPE_NONE};

static size_t get_option_array_len(const CmdOption* o) {
  size_t r = 0;
  CmdOption c;
  while((c=*o).full_name != NULL){
    r++;
    o++;
  }
  return r;
}

static void handle_help(const CmdOption* o, size_t olen) {

}

static const CmdOption* findopt_from_fname(const CmdOption* o, size_t olen, char* name) {
  if(strequal(name, "help")) {
    handle_help(o, olen);
    return &HELP_DUMMY;
  }
  for(size_t i=0; i<olen; i++) {
    if(strequal((o[i]).full_name, name)) {
      return o + i;
    }
  }
  return NULL;
}

static const CmdOption* findopt_from_sname(const CmdOption* o, size_t olen, char name) {
  if(name == 'h') {
    handle_help(o, olen);
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
  memcpy(cloned, s, l);
  cloned[l] = '\0';
  return cloned;
}

static void handle_argument(const CmdOption* o, const char* s) {
  switch(o->type){
    case SSCE_TYPE_STRING: *((char**)o->data) = clone_string(s);
      break;
    case SSCE_TYPE_INT32: *((int32_t*)o->data) = strtol(s, NULL, 0);
      break;
    case SSCE_TYPE_INT64: *((int64_t*)o->data) = strtoll(s, NULL, 0);
      break;
    case SSCE_TYPE_FLOAT: *((float*)o->data) = atof(s);
      break;
    case SSCE_TYPE_DOUBLE: ((CmdCallback)o->data)(o->full_name, s);
      break;
    case SSCE_TYPE_CALLBACK: 
      break;
    default: {
      fprintf(stderr, "Unknown enum: %i\nWrong library usage.\n", o->type);
      abort();
    }
  }
}

void parse_cmd_args(const CmdOption* o, int argc, char* argv[]) {
  size_t olen = get_option_array_len(o);
  for(int i=1; i<argc; i++) {
    char* cur = argv[i];
    if(cur[0] == '-') {
      const CmdOption* f;
      if(cur[1] == '-') {
        //Full option.
        f = findopt_from_fname(o, olen, cur+2);
      }
      else {
        //Half option.
        f = findopt_from_sname(o, olen, cur[1]);
      }
      if(f != NULL){
        if(f->type!=SSCE_TYPE_NONE || f->data!=NULL) {
          //Check if there are any arguments after this option.
          //If there are, collect them.
          if((++i) < argc) {
            handle_argument(f, argv[i]);
          }
          //Callbacks can handle zero arguments after option.
          else if(f->type == SSCE_TYPE_CALLBACK) {
            if(((CmdCallback)o->data)(f->full_name, NULL)){
              fprintf(stderr, "Callback failed while parsing %s\n", cur);
              exit(EXIT_FAILURE);
            }
          }
          else {
            fprintf(stderr, "Invalid arguments: Option %s takes one argument.\nUse --help (or -h) for help.", cur);
          }
        }
      }
      else {
        fprintf(stderr, "Invalid option: %s\nUse --help (or -h) for help.", cur);
        exit(EXIT_FAILURE);
      }
    }
    else {
      const CmdOption* def = findopt_from_fname(o, olen, SSCE_DEFAULT_NAME);
      if(def != NULL) {
        if(def->type!=SSCE_TYPE_NONE || def->data!=NULL) {
          handle_argument(def, cur);
        }
      }
      else {
        fprintf(stderr, "This program does not accept default arguments.\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}