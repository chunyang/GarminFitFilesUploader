#include "util.h"

/*
 * File header
 */
void fit_read_file_header(FIT_FILE_HDR *header, FILE *file)
{
    /* Find header length in first byte */
    fread(header, 1, 1, file);

    /* Read rest of header */
    fread(((uint8_t *) header) + 1, header->header_size - 1, 1, file);
}

void fit_print_file_header(FIT_FILE_HDR header)
{
    printf("=== File header ===\n");
    printf("Header size: %hhu\n", header.header_size);
    printf("Protocol Version: %hhu\n", header.protocol_version);
    printf("Profile Version: %hu\n", header.profile_version);
    printf("Data size: %u\n", header.data_size);
    printf("Data Type Byte: %c%c%c%c\n", header.data_type[0],
            header.data_type[1], header.data_type[2], header.data_type[3]);

    if (header.header_size > 12) {
        printf("CRC: %hhX\n", header.crc);
    }
    printf("=== End file header ===\n");
}

/*
 * Message
 */

/**
 * Read a message from the file
 *
 * \param[in] file Pointer to the .FIT file
 * \return Pointer to message, message storage is allocated by this function
 */
//fit_mesg* fit_read_message(FILE *file)
//{
//    fit_mesg_hdr header;
//    fread(&header, FIT_MESG_HDR_SIZE, 1, file);
//
//    /* Branch based on message header type */
//    if (FIT_MESG_HDR_TYPE(header) == FIT_MESG_NORMAL) {
//        printf("Message header type: normal\n");
//
//        printf("Message type: ");
//        if (FIT_MESG_HDR_MESG_TYPE(header) == FIT_MESG_DEFN) {
//            printf("definition\n");
//        } else {
//            printf("data\n");
//        }
//
//        printf("Local message type: %d\n",
//                FIT_MESG_HDR_LOCAL_MESG_TYPE(header));
//    } else {
//        printf("Message header type: compressed\n");
//        printf("Local message type: %d\n",
//                FIT_MESG_HDR_COMPRESSED_LOCAL_MESG_TYPE(header));
//        printf("Time offset (seconds): %d\n",
//                FIT_MESG_HDR_COMPRESSED_TIME_OFFSET(header));
//    }
//}
