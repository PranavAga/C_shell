#ifndef HEADERS_H_
#define HEADERS_H_

#define MULTI_COMMANDS ";"
#define BACKG_P "&"
#define PIPE_CMD "|"
#define RD_FILE "<"
#define OT_FILE ">"
#define OA_FILE ">>"

#define MAX_INP 4096
#define MAX_PATH 255
#define MAX_FILES 65535
#define MIN_TIMEDIFF 365*24*60*60-60
#define MAX_INHIST 15
#define FP_TIMELMT 2
#define OUT_PERMS 0644

typedef long file_size;

#include <stdio.h>
#include <unistd.h>
#include <pwd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>

#include "prompt.h"
#include "color.h"
#include "cerror.h"
#include "input.h"
#include "runcmd.h"
#include "commands.h"
#endif