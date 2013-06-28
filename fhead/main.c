#include <stdio.h>

#include "sdk/fit.h"
#include "util.h"

int main(int argc, const char *argv[])
{
    FILE *input;
    FIT_FILE_HDR header;

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
    fit_print_file_header(header);

    /* Read messages */
    //fit_read_message(input);

    fclose(input);

    return 0;
}
