#ifndef FAMP_CONFIG_DISK_IMAGE_H
#define FAMP_CONFIG_DISK_IMAGE_H
#include <FFF/FFF_structures.hpp>

/* Needed to check the entries when checking the MBR binary. */
#include <FFS/pt_entry_structures.hpp>

using namespace FFF_Structures;
using namespace PartitionTable_Entry_Structs;

namespace ConfigDiskImage
{
    cpint8 disk_image_path = (cpint8) initiate_path((pint8)"../bin/", (pint8)"OS.img");

    /* All binaries that the disk image will consist of. */
    cpint8 mbr_binary = (cpint8) initiate_path((pint8)"../bin/", (pint8)"mbr.bin");
    cpint8 mbr_part_table_bin = (cpint8) initiate_path((pint8)"../bin/", (pint8)"mbr_part_table.bin");
    cpint8 second_stage_bin = (cpint8) initiate_path((pint8)"../bin/", (pint8)"second_stage.bin");

    enum class program
    {
        MBR,
        MBR_PART_TABLE,
        SECOND_STAGE
    };

    template<typename T>
        requires (std::is_same<T, uint16_t>::value
            || std::is_same<T, uint32_t>::value)
            && (!std::is_same<T, uint8_t>::value)
    T revert_value(T &value)
    {
        T old_value = value;
        value ^= value;

        switch(sizeof(T))
        {
            case 2: {
                value |= (value << 0) | ((old_value >> 0) & 0xFF);
                value = (value << 8) | ((old_value >> 8) & 0xFF);
                break;
            }
            case 4: {
                value |= (value << 0) | ((old_value >> 0) & 0xFF);
                value = (value << 8) | ((old_value >> 8) & 0xFF);
                value = (value << 8) | ((old_value >> 16) & 0xFF);
                value = (value << 8) | ((old_value >> 24) & 0xFF);
                break;
            }
            default: break;
        }

        return (T) value;
    }

    class config_image
    {
    protected:
        struct FAMP_PROTOCOL_DISK_IMAGE_HEADING *dimg_heading = nullptr;
        puint8 disk_image_data = nullptr;
        size_t disk_image_size = 0;
        size_t disk_image_pos = 0;

    public:
        config_image() = default;

        ~config_image()
        {
            if(disk_image_data) delete disk_image_data;
            disk_image_data = nullptr;

            if(dimg_heading) delete dimg_heading;
            dimg_heading = nullptr;
        }
    };

    class adjust_binary : public config_image
    {
    private:
        /* Used to check MBR binary. */
        struct MBR_bin_outline
        {
            uint16          MBR_start_ID;
            uint8           MBR_start_ID_name[4];
            uint16          padding;

            /* There is 383 bytes (0x17F) of entry-code that is allowed between
             * the section data and the next piece of section data (MBR partition table entries).
             * */
            uint8           entry_code[0x17F];

            uint16          MBR_ptbl_entry_ID;
            uint8           MBR_ptbl_entry_ID_name[4];
            uint16          padding2;

            //uint8           rest_of_MBR[42];
        } __attribute__((packed));

        program binary_program;
        struct partition_entry *pentry = nullptr;

        size_t make_multiple_of_512(FILE *bin_file, bool has_subheading)
        {
            fseek(bin_file, 0, SEEK_END);
            size_t file_size = ftell(bin_file);
            fseek(bin_file, 0, SEEK_SET);

            if(has_subheading)
                file_size += FAMP_SUBHEADING_SIZE;
            
            size_t bytes_needed = file_size;
            while(bytes_needed % 512 != 0)
                bytes_needed++;
            


            disk_image_size += bytes_needed;
            disk_image_data = (puint8) realloc(
                disk_image_data,
                disk_image_size * sizeof(*disk_image_data)
            );

            return bytes_needed - file_size;
        }

    public:
        adjust_binary(program bin_program)
            : binary_program(bin_program)
        {
            FAMP_ASSERT(binary_program == program::MBR,
                "\nCannot initiate `adjust_binary` with anything other than `program::MBR`.\n")
            disk_image_data = (puint8) calloc(512, sizeof(*disk_image_data));
        }

        void switch_binary_program(program new_program)
        {
            FAMP_ASSERT(new_program != binary_program,
                "\nAlready adjusted the program that was passed to `switch_binary_program`.\n")
            
            binary_program = new_program;
        }

        void adjust()
        {
            struct FAMP_PROTOCOL_SUBHEADING *sheading = nullptr;
            FILE *bin_file = nullptr;
            puint8 bin_file_data = nullptr;

            switch(binary_program)
            {
                case program::MBR: {
                    bin_file = fopen(mbr_binary, "rb");
                    FAMP_ASSERT(bin_file,
                        "\nError opening up `%s`.\n", mbr_binary)
                    
                    dimg_heading = new struct FAMP_PROTOCOL_DISK_IMAGE_HEADING;
                    fread(dimg_heading, 1, sizeof(*dimg_heading), bin_file);

                    /* Read in the rest of the MBR binary and check the sections. */
                    {
                        struct MBR_bin_outline *mbr_outline = new struct MBR_bin_outline;
                        
                        fseek(bin_file, sizeof(*dimg_heading) - 1, SEEK_SET);
                        fread(mbr_outline, 1, sizeof(*mbr_outline), bin_file);

                        /* Check the data. */
                        FAMP_ASSERT((mbr_outline->MBR_start_ID == FAMP_MBR_START_ID) &&
                                    (strcmp((cpint8) mbr_outline->MBR_start_ID_name, "FENT") == 0) &&
                                    (mbr_outline->MBR_ptbl_entry_ID == FAMP_MBR_PTBL_ENTRIES_ID) &&
                                    (strcmp((cpint8) mbr_outline->MBR_ptbl_entry_ID_name, "FMPT") == 0),
                            "\nInvalid MBR start ID in section found.\n")

                        delete mbr_outline;
                        mbr_outline = nullptr;

                        /* check the entries. */
                        pentry = new struct partition_entry;
                        
                        {
                            /* Entry 1 - Second Stage Bootloader. */
                            fread(pentry, 1, sizeof(*pentry), bin_file);

                            FAMP_ASSERT(pentry->bootable_entry == ENTRY_IS_BOOTABLE,
                                "\nError with first entry in the MBR partition table entry:\n\tThe entry was marked not bootable.\n")
                            FAMP_ASSERT(pentry->starting_sector == 0x03,
                                "\nError with first entry in the MBR partition table entry:\n\tThe second stage gets read in from the third to fifth sector.\n\tThis data was not found in the entry description.\n")
                            FAMP_ASSERT(pentry->entry_type == ENTRY_TYPE_SS,
                                "\nError with first entry in the MBR partition table entry:\n\tThe second stage entry type that gets referenced by the partition entry was not found to be 0x0E.\n")
                            FAMP_ASSERT(pentry->auto_read_program == true,
                                "\nError with first entry in the MBR partition table entry:\n\tThe second stage program is auto read by the MBR partition table C++ program.\n\tIt was found to be set to false in the entry.\n")
                            FAMP_ASSERT(pentry->last_sector == 0x05,
                                "\nError with first entry in the MBR partition table entry:\n\tThe second stage bootloader consists of two sectors (from the third to the fifth).\n\tThis data was not found in the entry.")
                        }

                        {
                            /* TODO: Read the partition table entry that describes information about the filesystem. */
                        }

                        delete pentry;
                        pentry = nullptr;

                        disk_image_size += 512;
                        disk_image_pos = 512;
                    }

                    fseek(bin_file, 0, SEEK_SET);
                    
                    /* For some reason, the header signature gets read in wrong. Rever the value. */
                    revert_value<uint32_t> (dimg_heading->HeaderSig);

                    fread(disk_image_data, 512, sizeof(*disk_image_data), bin_file);
                    fclose(bin_file);

                    return;
                }
                case program::MBR_PART_TABLE: {
                    bin_file = fopen(mbr_part_table_bin, "rb");
                    FAMP_ASSERT(bin_file,
                        "\nError opening up `%s`.\n", mbr_part_table_bin)
                    
                    struct FAMP_PROTOCOL_MEMORY_STAMP mem_stamp;

                    /* Memory stamp information. */
                    mem_stamp.MemID = FAMP_MEM_STAMP_PTBLE_P;
                    mem_stamp.MemIDSig = revert_value<uint32_t> (FAMP_MEM_STAMP_FEND_SIG);
                    mem_stamp.padding = 0x0000;

                    /* Make sure the binary file is a multiple of 512. */
                    size_t padding = make_multiple_of_512(bin_file, false) - sizeof(mem_stamp); /* 8 bytes for memory stamp */
                    uint8 padding_value[padding];
                    memset(padding_value, 0, padding);

                    /* Read in the binary data that will be written before the padding/memory stamp. */
                    uint8 bin_data[disk_image_size - 512];
                    memset(bin_data, 0, (disk_image_size - 512) - padding);
                    fread(&bin_data, (disk_image_size - 512) - padding, sizeof(uint8), bin_file);
                    fclose(bin_file);

                    /* Open the binary file to be written to. */
                    bin_file = fopen(mbr_part_table_bin, "wb");
                    
                    /* Write the binary data back to the file. */
                    fwrite(&bin_data, (disk_image_size - 512) - padding, sizeof(uint8), bin_file);
                    
                    {
                        /* Write the padding/memory stamp to the file. */
                        fwrite(&padding_value, padding-sizeof(mem_stamp), sizeof(uint8), bin_file);
                        fwrite(&mem_stamp, 1, sizeof(mem_stamp), bin_file);
                    }
                    
                    fclose(bin_file);

                    return;
                }
                case program::SECOND_STAGE: {
                    sheading = new struct FAMP_PROTOCOL_SUBHEADING;

                    sheading->SubHeadingSig = FAMP_SUBHEADER_SIGNATURE;
                    sheading->padding = 0x0000;

                    delete bin_file_data;
                    bin_file_data = nullptr;

                    delete sheading;
                    sheading = nullptr;
                    return;
                }
                default: FAMP_ERROR("\nUnknown error occurred.\n")
            }
        }

        template<typename T>
            requires std::is_same<T, adjust_binary *>::value
                || std::is_same<T, config_image>::value
                || std::is_same<T, config_image *>::value
        void delete_instance(T instance)
        {
            if(instance)
                delete instance;
            instance = nullptr;
        }

        ~adjust_binary()
        {
            if(disk_image_data) delete disk_image_data;
            disk_image_data = nullptr;
        }
    };
}

#endif