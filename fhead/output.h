#ifndef OUTPUT_H
#define OUTPUT_H

#include <stdio.h>
#include <string.h>

#include "sdk/fit.h"
#include "sdk/fit_crc.h"
#include "sdk/fit_product.h"

void write_header(FILE *fp);

void write_data(const void *data, FIT_UINT8 data_size, FILE *fp);

FIT_UINT8 write_mesg_defn(FIT_MESG_NUM mesg_num, FILE *fp);

void write_mesg_data(FIT_UINT8 local_mesg_num,
        const FIT_UINT8 *mesg, FIT_MESG_NUM global_mesg_num, FILE *fp);

void write_mesg(const FIT_UINT8* mesg, FIT_MESG_NUM mesg_num, FILE *fp);

void write_crc(FILE *fp);

#endif
