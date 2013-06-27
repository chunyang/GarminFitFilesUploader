#include <stdio.h>

#include "fit.h"

int main(int argc, const char *argv[])
{
    FILE *input;
    fit_file_hdr header;

    if (argc < 2) {
        printf("Usage: %s <.FIT file>\n", argv[0]);
        return 0;
    }

    /* Open file for reading */
    input = fopen(argv[1], "rb");

    if (!input) {
        fprintf(stderr, "Cannot open input file %s, aborting.\n", argv[1]);
        return 1;
    }

    /* Read FIT file header */
    fit_read_file_header(&header, input);

    /* Print out header information */
    printf("Header size: %hhu\n", header.header_size);
    printf("Protocol Version: %hhu\n", header.protocol_version);
    printf("Profile Version: %hu\n", header.profile_version);
    printf("Data size: %u\n", header.data_size);
    printf("Data Type Byte: %c%c%c%c\n", header.data_type[0],
            header.data_type[1], header.data_type[2], header.data_type[3]);

    if (header.header_size > 12) {
        printf("CRC: %hhX\n", header.crc);
    }

    /* Read messages */
    fit_read_message(input);

    fclose(input);

    return 0;
}
