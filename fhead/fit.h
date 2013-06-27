#ifndef FIT_H
#define FIT_H

#include <stdio.h>
#include <stdint.h>

/*
 * File header
 */

typedef struct {
   uint8_t header_size;         // size of header, >= 12 bytes
   uint8_t protocol_version;
   uint16_t profile_version;
   uint32_t data_size;          // Does not include file header or crc.  Little endian format.
   uint8_t data_type[4];        // ".FIT"
   uint16_t crc;                // CRC of this file header in little endian format. (Optional)
}__attribute__((packed)) fit_file_hdr;

#define FIT_FILE_HDR_SIZE 14

/*
 * Message header
 */

typedef uint8_t fit_mesg_hdr;
#define FIT_MESG_HDR_TYPE(h) (((h) & 0x80) >> 7)
#define FIT_MESG_HDR_MESG_TYPE(h) (((h) & 0x40) >> 6)
#define FIT_MESG_HDR_LOCAL_MESG_TYPE(h) ((h) & 0xF)
#define FIT_MESG_HDR_COMPRESSED_LOCAL_MESG_TYPE(h) (((h) & 0x60) >> 5)
#define FIT_MESG_HDR_COMPRESSED_TIME_OFFSET(h) ((h) & 0x1F)

typedef enum {
    FIT_MESG_NORMAL = 0,
    FIT_MESG_COMPRESSED = 1
} fit_mesg_hdr_type;

typedef enum {
    FIT_MESG_DATA = 0,
    FIT_MESG_DEFN = 1
} fit_mesg_hdr_mesg_type;

#define FIT_MESG_HDR_SIZE 1

/*
 * Functions
 */

void fit_read_file_header(fit_file_hdr *, FILE *);

void fit_read_message(FILE *);

#endif
