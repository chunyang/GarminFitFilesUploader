#include "fit.h"

void fit_read_file_header(fit_file_hdr *header, FILE *file)
{
    /* Find header length in first byte */
    fread(header, 1, 1, file);

    /* Read rest of header */
    fread(((uint8_t *) header) + 1, header->header_size - 1, 1, file);
}

void fit_read_message(FILE *file)
{
    fit_mesg_hdr header;
    fread(&header, FIT_MESG_HDR_SIZE, 1, file);

    /* Branch based on message header type */
    if (FIT_MESG_HDR_TYPE(header) == FIT_MESG_NORMAL) {
        printf("Message header type: normal\n");

        printf("Message type: ");
        if (FIT_MESG_HDR_MESG_TYPE(header) == FIT_MESG_DEFN) {
            printf("definition\n");
        } else {
            printf("data\n");
        }

        printf("Local message type: %d\n",
                FIT_MESG_HDR_LOCAL_MESG_TYPE(header));
    } else {
        printf("Message header type: compressed\n");
        printf("Local message type: %d\n",
                FIT_MESG_HDR_COMPRESSED_LOCAL_MESG_TYPE(header));
        printf("Time offset (seconds): %d\n",
                FIT_MESG_HDR_COMPRESSED_TIME_OFFSET(header));
    }

}
