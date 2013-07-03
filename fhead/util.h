#ifndef UTIL_H
#define UTIL_H

#include <arpa/inet.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "sdk/fit.h"
#include "sdk/fit_convert.h"

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

void fit_compare_file_id(FIT_FILE_ID_MESG *,
                         FIT_FILE_ID_MESG *,
                         unsigned int);
void fit_compare_file_creator(FIT_FILE_CREATOR_MESG *,
                              FIT_FILE_CREATOR_MESG *,
                              unsigned int);
void update_activity(FIT_ACTIVITY_MESG *, FIT_ACTIVITY_MESG *, unsigned int);

FIT_MESG_DEF* fit_read_message(FILE *);

#endif
