#ifndef HEADERS_H_
#define HEADERS_H_

#define MAX_PATH 255
#define MAX_FILES 65535
#define MIN_TIMEDIFF 365*24*60*60-60

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

#include "prompt.h"
#include "color.h"
#include "cerror.h"
#include "input.h"
#include "runcmd.h"
#include "commands.h"
#endif