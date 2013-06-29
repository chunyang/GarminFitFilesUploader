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
FIT_MESG_DEF* fit_read_message(FILE *file)
{
    FIT_UINT8 header;
    FIT_MESG_DEF *def = NULL;
    fread(&header, FIT_HDR_SIZE, 1, file);

    printf("=== Message ===\n");

    /* Branch based on message header type */
    if (header & FIT_HDR_TIME_REC_BIT) {
        /* Compressed message header */
        printf("Message header type: compressed\n");

        printf("Local message type: %d\n",
                (header & FIT_HDR_TIME_TYPE_MASK) >> FIT_HDR_TIME_TYPE_SHIFT);

        printf("Time offset (seconds): %d\n",
                header & FIT_HDR_TIME_OFFSET_MASK);
    } else {
        /* Normal message header */
        printf("Message header type: normal\n");

        printf("Message type: ");
        if (header & FIT_HDR_TYPE_DEF_BIT) {
            printf("definition\n");
        } else {
            /* Data message */
            printf("data\n");
        }

        printf("Local message type: %d\n", header & FIT_HDR_TYPE_MASK);

        if (header & FIT_HDR_TYPE_DEF_BIT) {
            /* Definition message */
            def = malloc(FIT_MESG_SIZE_NO_FIELDS);

            if (!def) {
                fprintf(stderr, "Unable to allocate space for message.\n");
                return NULL;
            }

            fread(def, FIT_MESG_SIZE_NO_FIELDS, 1, file);

            printf("Architecture type: ");
            if (def->arch) {
                printf("big endian\n");
            } else {
                printf("little endian\n");
            }

            printf("Global message number: ");
            if (def->arch) {
                printf("%d\n", htons(def->global_mesg_num));
            } else {
                printf("%d\n", def->global_mesg_num);
            }

            printf("Number of fields: %d\n", def->num_fields);

            /* Reallocate space for message with the correct number
             * of fields
             */
            FIT_MESG_DEF *def2;
            def2 = realloc(def, FIT_MESG_SIZE_NO_FIELDS +
                                (def->num_fields * FIT_FIELD_DEF_SIZE));

            if (!def2) {
                fprintf(stderr, "Unable to reallocate space for message.\n");
                return NULL;
            }
            def2 = def;

            fread(def->fields, FIT_FIELD_DEF_SIZE, def->num_fields, file);

            printf("Fields:\n");
            unsigned int i;
            for (i = 0; i < def->num_fields; i++) {
                printf("  === Field ===\n");

                FIT_FIELD_DEF *field_def = ((FIT_FIELD_DEF*) def->fields) + i;

                printf("  Field definition number: %d\n",
                        field_def->field_def_num);
                printf("  Size: %d\n", field_def->size);
                printf("  Endian ability: %d\n",
                        (field_def->base_type & FIT_BASE_TYPE_ENDIAN_FLAG) >> 7);
                printf("  Base type number: %d\n",
                        field_def->base_type & FIT_BASE_TYPE_NUM_MASK);

                printf("  === End of field ===\n");
            }
        }
    }

    printf("=== End of message ===\n");

    return def;
}
