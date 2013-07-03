#include "output.h"

static FIT_UINT16 data_crc;

extern FIT_MESG_NUM local_mesg_nums[FIT_MAX_LOCAL_MESGS];
extern unsigned int local_mesg_num_next;

void write_header(FILE *fp)
{
    FIT_FILE_HDR file_header;

    file_header.header_size = FIT_FILE_HDR_SIZE;
    file_header.profile_version = FIT_PROFILE_VERSION;
    file_header.protocol_version = FIT_PROTOCOL_VERSION;
    memcpy((FIT_UINT8 *)&file_header.data_type, ".FIT", 4);
    fseek (fp , 0 , SEEK_END);
    file_header.data_size = ftell(fp) - FIT_FILE_HDR_SIZE - sizeof(FIT_UINT16);
    file_header.crc = FitCRC_Calc16(&file_header, FIT_STRUCT_OFFSET(crc, FIT_FILE_HDR));   

    fseek (fp , 0 , SEEK_SET);
    fwrite((void *)&file_header, 1, FIT_FILE_HDR_SIZE, fp);
}

/*
 * Write data while calculating crc
 */
void write_data(const void *data, FIT_UINT8 data_size, FILE *fp)
{
   FIT_UINT8 offset;

   fwrite(data, 1, data_size, fp);

   for (offset = 0; offset < data_size; offset++)
      data_crc = FitCRC_Get16(data_crc, *((FIT_UINT8 *)data + offset));
}

/*
 * If definition has not been written, write it and return a new local message
 * number. Otherwise return the message number corresponding to the message.
 */
FIT_UINT8 write_mesg_defn(FIT_MESG_NUM mesg_num, FILE *fp)
{
    int i;
    FIT_UINT8 local_mesg_num;

    /* See if definition has been written */
    for (i = 0; i < FIT_MAX_LOCAL_MESGS; i++) {
        if (local_mesg_nums[i] == mesg_num) {
            return i;
        }
    }

    /* Definition has not been written, so write it and return new local
     * message number
     */
    local_mesg_num = local_mesg_num_next;
    local_mesg_num_next = (local_mesg_num_next + 1) % FIT_MAX_LOCAL_MESGS;

    /* Definition header */
    FIT_UINT8 header = local_mesg_num | FIT_HDR_TYPE_DEF_BIT;

    /* Definition */
    const FIT_MESG_DEF *def = Fit_GetMesgDef(mesg_num);
    // switch (mesg_num) {
    //     case FIT_MESG_NUM_SESSION:
    //         def = fit_mesg_defs[FIT_MESG_SESSION];
    //         break;
    //     case FIT_MESG_NUM_LAP:
    //         def = fit_mesg_defs[FIT_MESG_LAP];
    //         break;
    //     case FIT_MESG_NUM_RECORD:
    //         def = fit_mesg_defs[FIT_MESG_RECORD];
    //         break;
    //     case FIT_MESG_NUM_EVENT:
    //         def = fit_mesg_defs[FIT_MESG_EVENT];
    //         break;
    //     default:
    //         fprintf(stderr, "Don't know how to write definition for message "
    //                 "number %hd\n", mesg_num);
    //         return -1;
    // }

    /* Write header and definition */
    write_data(&header, FIT_HDR_SIZE, fp);
    write_data(def, Fit_GetMesgDefSize(def), fp);

    /* Update local mesg num mapping */
    local_mesg_nums[local_mesg_num] = mesg_num;
    printf("Wrote definition for local message number %hhd "
            "(matches global number %hd)\n",
            local_mesg_num, mesg_num);

    return local_mesg_num;
}

void write_mesg_data(FIT_UINT8 local_mesg_num,
        const FIT_UINT8 *mesg, FIT_MESG_NUM global_mesg_num, FILE *fp)
{
    if (local_mesg_num < 0) {
        fprintf(stderr, "Invalid local message number, not writing data.\n");
        return;
    }

    write_data(&local_mesg_num, FIT_HDR_SIZE, fp);
    write_data(mesg, Fit_GetMesgSize(global_mesg_num), fp);

    printf("Wrote data for local message number %hhd\n", local_mesg_num);
}

void write_mesg(const FIT_UINT8 *mesg, FIT_MESG_NUM mesg_num, FILE *fp)
{
    FIT_UINT8 local_mesg_num = write_mesg_defn(mesg_num, fp);
    write_mesg_data(local_mesg_num, mesg, mesg_num, fp);
}

void write_crc(FILE *fp)
{
   fwrite(&data_crc, 1, sizeof(FIT_UINT16), fp);
}
