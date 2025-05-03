
#ifndef __LOCAL_H_
#define __LOCAL_H_

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#define BUFSIZ 100
#define SEED   'g'
typedef struct {
  int message_type;		/* Placed in the queue for */
  long msg_to;		/* The process to receive */
  long       msg_fm;		/* Placed in the queue by  */
  char      buffer[BUFSIZ];
} MESSAGE;

#endif