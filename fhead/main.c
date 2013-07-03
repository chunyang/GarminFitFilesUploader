#include <stdio.h>
#include <string.h>

#include "sdk/fit.h"
#include "sdk/fit_product.h"
#include "output.h"
#include "util.h"

FIT_MESG_NUM local_mesg_nums[FIT_MAX_LOCAL_MESGS];
unsigned int local_mesg_num_next;

int main(int argc, const char *argv[])
{
    /* For reading input files */
    FILE *file;
    FIT_UINT8 buf[8];
    FIT_CONVERT_RETURN convert_return = FIT_CONVERT_CONTINUE;
    FIT_UINT32 buf_size;
    FIT_UINT32 mesg_index = 0;

    /* For writing output files */
    FILE *out_file;
    int i;
    for (i = 0; i < FIT_MAX_LOCAL_MESGS; i++) {
        local_mesg_nums[i] = FIT_MESG_NUM_INVALID;
    }

    /* Keep track of file, device, and activity information */
    FIT_FILE_ID_MESG file_id;
    FIT_FILE_CREATOR_MESG file_creator;
    FIT_ACTIVITY_MESG activity;
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

    /* Initialize FitConvert, read_file_header = TRUE */
    FitConvert_Init(FIT_TRUE);


    /* Open file for writing */
    out_file = fopen("output.fit", "wb");

    if (!out_file) {
        fprintf(stderr, "Cannot open output file %s, aborting.\n",
                "output.fit");
        fclose(file);
        return 1;
    }

    /* Write output file header */
    write_header(out_file);

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

                    mesg_index++;
                    // printf("Message %d (%d) - \n", mesg_index++, mesg_num);
                    // printf("Message available!\n");

                    switch(mesg_num) {
                        case FIT_MESG_NUM_FILE_ID:
                            fit_compare_file_id((FIT_FILE_ID_MESG*) mesg,
                                    &file_id, file_number);
                            if (file_number == 0) {
                                write_mesg((const FIT_UINT8*) &file_id,
                                        mesg_num, out_file);
                            }
                            break;
                        case FIT_MESG_NUM_FILE_CREATOR:
                            fit_compare_file_creator((FIT_FILE_CREATOR_MESG*) mesg,
                                    &file_creator, file_number);
                            if (file_number == 0) {
                                write_mesg((const FIT_UINT8*) &file_creator,
                                        mesg_num, out_file);
                            }
                            break;
                        case FIT_MESG_NUM_DEVICE_INFO:
                            if (file_number == 0) {
                                write_mesg(mesg, mesg_num, out_file);
                            }
                            break;
                        case FIT_MESG_NUM_SESSION:
                        case FIT_MESG_NUM_LAP:
                        case FIT_MESG_NUM_RECORD:
                        case FIT_MESG_NUM_EVENT:
                            write_mesg(mesg, mesg_num, out_file);
                            break;
                        case FIT_MESG_NUM_ACTIVITY:
                            update_activity((FIT_ACTIVITY_MESG*) mesg,
                                    &activity, file_number);
                            if (file_number >= argc - 2) {
                                /* This is the last file */
                                write_mesg((const FIT_UINT8*) &activity,
                                        mesg_num, out_file);
                            }
                            printf("Wrote activity\n");
                            break;
                        default:
                            fprintf(stderr, "Unknown message number: %hd\n",
                                    mesg_num);
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

    /* Update header with appropriate file size and CRC */
    write_crc(out_file);
    write_header(out_file);

    fclose(file);
    fclose(out_file);

    return 0;
}
