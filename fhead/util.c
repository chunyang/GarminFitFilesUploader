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
 * Parse a file id message and compare it with previous file id info
 *
 * \param[in] new_id Pointer to new file id message
 * \param[in] old_id Pointer to saved file id information
 * \param[in] file_number The index of the current file (first file is 0)
 */
void fit_compare_file_id(FIT_FILE_ID_MESG *new_id, FIT_FILE_ID_MESG *old_id,
        unsigned int file_number)
{
    if (file_number == 0) {
        /* This is the first file, just store its file
         * id message
         */
         memcpy(old_id, new_id, FIT_FILE_ID_MESG_SIZE);
    } else {
        /* This is not the first file, check to see
         * if type, manufacturer, and product are
         * the same
         */
        if (old_id->type != new_id->type) {
            fprintf(stderr, "File types do not match (%hhd vs %hhd). Aborting.\n",
                    old_id->type, new_id->type);
            exit(5);
        }

        if (old_id->manufacturer != new_id->manufacturer) {
            fprintf(stderr, "Warning: manufacturer does not match (%hd vs %hd). Ignoring.\n",
                    old_id->manufacturer, new_id->manufacturer);
        }

        if (old_id->product != new_id->product) {
            fprintf(stderr, "Warning: product does not match (%hd vs %hd). Ignoring.\n",
                    old_id->product, new_id->product);
        }

        if (old_id->time_created > new_id->time_created) {
            fprintf(stderr, "Warning: latter file has earlier creation time.\n");
            old_id->time_created = new_id->time_created;
        }
    }
}

/**
 * Parse a file creator message and compare with previous file creator info.
 *
 * \param[in] new_creator Pointer to new file creator message
 * \param[in] old_creator Pointer to saved file creator information
 * \param[in] file_number The index of the current file (first file is 0)
 */
void fit_compare_file_creator(FIT_FILE_CREATOR_MESG *new_creator,
        FIT_FILE_CREATOR_MESG *old_creator, unsigned int file_number)
{
    if (file_number == 0) {
        /* No previous creator, just copy new info */
        memcpy(old_creator, new_creator, FIT_FILE_CREATOR_MESG_SIZE);
    } else {
        if (new_creator->software_version != old_creator->software_version) {
            fprintf(stderr, "Warning: software versions differ.\n");
        }
        if (new_creator->hardware_version != old_creator->hardware_version) {
            fprintf(stderr, "Warning: hardware versions differ.\n");
        }
    }
}

/**
 * Update activity information when concatenating files
 *
 * \param[in] new_act Pointer to new activity message
 * \param[in] old_act Pointer to saved activity information
 * \param[in] file_number The index of the current file (first file is 0)
 */
void update_activity(FIT_ACTIVITY_MESG *new_act,
        FIT_ACTIVITY_MESG *old_act, unsigned int file_number)
{
    if (file_number == 0) {
        /* No previous activity info, just copy new info */
        memcpy(old_act, new_act, FIT_ACTIVITY_MESG_SIZE);
    } else {
        old_act->total_timer_time += new_act->total_timer_time;
        old_act->num_sessions += new_act->num_sessions;
        /* TODO: Check other fields?? */
    }
}

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
