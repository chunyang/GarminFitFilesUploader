#ifndef UTIL_H
#define UTIL_H

#include <arpa/inet.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "sdk/fit.h"

/*
 * File header
 */

/*
 * Message header
 */
#define FIT_MESG_SIZE_NO_FIELDS 5

/*
 * Functions
 */

void fit_read_file_header(FIT_FILE_HDR *, FILE *);
void fit_print_file_header(FIT_FILE_HDR);

FIT_MESG_DEF* fit_read_message(FILE *);

#endif
