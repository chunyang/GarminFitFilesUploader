#include <stdio.h>
#include <string.h>

#include "sdk/fit.h"
#include "util.h"

int main(int argc, const char *argv[])
{
    FILE *file;
    FIT_UINT8 buf[8];
    FIT_CONVERT_RETURN convert_return = FIT_CONVERT_CONTINUE;
    FIT_UINT32 buf_size;
    FIT_UINT32 mesg_index = 0;

    /* Keep track of file and device information */
    FIT_FILE_ID_MESG file_id;
    FIT_FILE_CREATOR_MESG file_creator;
    unsigned int file_number = 0;

    if (argc < 2) {
        printf("Usage: %s <.FIT file>\n", argv[0]);
        return 0;
    }

    /* Open file for reading */
    file = fopen(argv[1], "rb");

    if (!file) {
        fprintf(stderr, "Cannot open input file %s, aborting.\n", argv[1]);
        return 1;
    }

    // /* Read FIT file header */
    // fit_read_file_header(&header, file);

    // /* Print out header information */
    // fit_print_file_header(header);

    // Initialize FitConvert, read_file_header = TRUE
    FitConvert_Init(FIT_TRUE);

    /* Read messages */
    while (!feof(file) && (convert_return == FIT_CONVERT_CONTINUE)) {
        for (buf_size = 0; (buf_size < sizeof buf) && !feof(file); buf_size++) {
            buf[buf_size] = getc(file);
        }

        do {
            convert_return = FitConvert_Read(buf, buf_size);

            switch(convert_return) {
                case FIT_CONVERT_MESSAGE_AVAILABLE:
                {
                    const FIT_UINT8 *mesg = FitConvert_GetMessageData();
                    FIT_UINT16 mesg_num = FitConvert_GetMessageNumber();

                    printf("Message %d (%d) - \n", mesg_index++, mesg_num);
                    printf("Message available!\n");

                    switch(mesg_num) {
                        case FIT_MESG_NUM_FILE_ID:
                            fit_compare_file_id((FIT_FILE_ID_MESG*) mesg,
                                    &file_id, file_number);
                            break;
                        case FIT_MESG_NUM_FILE_CREATOR:
                            fit_compare_file_creator((FIT_FILE_CREATOR_MESG*) mesg,
                                    &file_creator, file_number);
                            break;
                    }
                }

                default:
                    break;
            }
        } while (convert_return == FIT_CONVERT_MESSAGE_AVAILABLE);
    }

    switch(convert_return) {
        case FIT_CONVERT_ERROR:
            fprintf(stderr, "Error decoding file.\n");
            fclose(file);
            return 2;
        case FIT_CONVERT_CONTINUE:
            fprintf(stderr, "Unexpected end of file.\n");
            fclose(file);
            return 3;
        case FIT_CONVERT_PROTOCOL_VERSION_NOT_SUPPORTED:
            fprintf(stderr, "Protocol version not supported.\n");
            fclose(file);
            return 4;
        case FIT_CONVERT_END_OF_FILE:
            printf("File converted successfully.\n");
            break;
        default:
            printf("Fit convert: %d\n", convert_return);
            break;
    }

    fclose(file);

    return 0;
}
