#ifndef FAMP_CONFIG_DISK_IMAGE_H
#define FAMP_CONFIG_DISK_IMAGE_H
#include <FFF/FFF_structures.hpp>

/* Needed to check the entries when checking the MBR binary. */
#include <bootloader/bootloader_structures.hpp>

using namespace FFF_Structures;
using namespace BootloaderStructs;

namespace ConfigDiskImage
{
    cpint8 disk_image_path = (cpint8) initiate_path((pint8)"../bin/", (pint8)"OS.img");

    /* All binaries that the disk image will consist of. */
    cpint8 mbr_binary = (cpint8) initiate_path((pint8)"../bin/", (pint8)"mbr.bin");
    cpint8 mbr_part_table_bin = (cpint8) initiate_path((pint8)"../bin/", (pint8)"mbr_part_table.bin");
    cpint8 fs_worker_bin = (cpint8) initiate_path((pint8) "../bin/", (pint8)"fs_worker.bin");
    cpint8 second_stage_bin = (cpint8) initiate_path((pint8)"../bin/", (pint8)"second_stage.bin");
    cpint8 fs_bin = (cpint8) initiate_path((pint8)"../bin/", (pint8) "fs.bin");

    uint8 kernel_bin_path[40] = "../../%s";

    /* `kernel_bin_path` is the "formatter" that `abs_kernel_bin_path`
     * will be formatted with.
     * */
    uint8 abs_kernel_bin_path[40];

    enum class program
    {
        MBR,
        MBR_PART_TABLE,
        FS_WORKER,
        SECOND_STAGE,
        KERNEL,
        FILESYSTEM
    };

    class config_image
    {
    protected:
        struct FAMP_PROTOCOL_DISK_IMAGE_HEADING *dimg_heading = nullptr;
        puint8 disk_image_data = nullptr;
        size_t disk_image_size = 0;
        size_t disk_image_pos = 0;

    public:
        config_image() = default;

        void write_disk_image()
        {
            FILE *disk_image_file = fopen(disk_image_path, "wb");

            fwrite(disk_image_data, disk_image_pos, sizeof(*disk_image_data), disk_image_file);
            fclose(disk_image_file);
        }

        ~config_image()
        {
            if(disk_image_data) delete disk_image_data;
            if(dimg_heading) delete dimg_heading;
        }
    };

    class adjust_binary : public config_image
    {
    private:
        /* Used to check MBR binary. */

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

            if(new_program == program::KERNEL)
                sprintf((pint8) abs_kernel_bin_path, (cpint8)kernel_bin_path,
                    yod.kernel_bin_filename);
        }

        void adjust()
        {
            struct FAMP_PROTOCOL_SUBHEADING *sheading = nullptr;
            FILE *bin_file = nullptr;
            puint8 bin_file_data = nullptr;
            puint8 padding_val = nullptr;
            struct FAMP_PROTOCOL_MEMORY_STAMP mem_stamp;

            const auto create_padding = [&padding_val, this] (FILE* bin_file, bool has_subheading)
            {
                size_t padding = make_multiple_of_512(bin_file, has_subheading) - sizeof(struct FAMP_PROTOCOL_MEMORY_STAMP);
                padding_val = new uint8[padding];
                
                return padding;
            };

            constexpr auto get_file_size = [] (FILE *bin_file)
            {
                fseek(bin_file, 0, SEEK_END);
                size_t file_size = ftell(bin_file);
                fseek(bin_file, 0, SEEK_SET);

                return file_size;
            };

            const auto get_file_data = [&bin_file_data, &bin_file, &get_file_size] ()
            {
                size_t file_size = get_file_size(bin_file);

                if(bin_file_data)
                    free(bin_file_data);

                bin_file_data = (puint8) calloc(file_size, sizeof(*bin_file_data));
                fread(bin_file_data, file_size, sizeof(*bin_file_data), bin_file);
                return file_size;
            };

            const auto init_adjustment = [&bin_file, &mem_stamp, &create_padding] (cpint8 filename, uint16 mem_id, bool has_subheading)
            {
                bin_file = fopen(filename, "rb");
                FAMP_ASSERT(bin_file,
                    "\nError opening `%s`.\n", filename)
                
                size_t padding = create_padding(bin_file, has_subheading);

                return padding;
            };

            constexpr auto init_subheading = [] (struct FAMP_PROTOCOL_SUBHEADING &sheader, uint32 psize_in_bytes, uint16 psize_in_sectors, bool is_asm_program)
            {
                sheader.SubHeadingSig = revert_value<uint32> (FAMP_SUBHEADER_SIGNATURE);
                sheader.padding = 0x0000;
                sheader.ProgramSizeInBytes = psize_in_bytes;
                sheader.ProgramSizeInSectors = psize_in_sectors;
                sheader.IsAsmProgram = is_asm_program;
            };

            const auto read_into_disk_image = [&bin_file, &get_file_size, this] (cpint8 filename)
            {
                bin_file = fopen(filename, "rb");

                disk_image_data = (puint8) realloc(
                    disk_image_data,
                    (disk_image_size + get_file_size(bin_file)) * sizeof(*disk_image_data)
                );

                fread(&disk_image_data[disk_image_pos], get_file_size(bin_file), sizeof(*disk_image_data), bin_file);
                fseek(bin_file, 0, SEEK_SET);

                disk_image_pos += get_file_size(bin_file);
                fclose(bin_file);
            };

            const auto write_new_binary = [&bin_file_data, &bin_file, &padding_val] (bool has_subheading, struct FAMP_PROTOCOL_SUBHEADING &sheading, struct FAMP_PROTOCOL_MEMORY_STAMP mem_stamp, size_t data_size, size_t padding_size)
            {
                if(has_subheading)
                    fwrite(&sheading, 1, sizeof(struct FAMP_PROTOCOL_SUBHEADING), bin_file);
                
                fwrite(bin_file_data, data_size, sizeof(*bin_file_data), bin_file);
                fwrite(padding_val, padding_size, sizeof(uint8), bin_file);
                fwrite(&mem_stamp, 1, sizeof(mem_stamp), bin_file);
                fclose(bin_file);

                delete padding_val;
            };

            switch(binary_program)
            {
                case program::MBR: {
                    bin_file = fopen(mbr_binary, "rb");
                    FAMP_ASSERT(bin_file,
                        "\nError opening up `%s`.\n", mbr_binary)
                    
                    dimg_heading = new struct FAMP_PROTOCOL_DISK_IMAGE_HEADING;

                    /* The `jmp` instruction at the beginning of the disk image
                     * results in 5 bytes. Skip the 5 bytes.
                     * At the 6th byte, the disk image header will begin.
                     * */
                    fseek(bin_file, FAMP_DISK_IMAGE_HDR_OFFSET, SEEK_SET);
                    fread(dimg_heading, 1, sizeof(*dimg_heading), bin_file);

                    /* Read in the rest of the MBR binary and check the sections. */
                    {
                        struct MBR_bin_outline *mbr_outline = new struct MBR_bin_outline;
                        
                        fseek(bin_file, sizeof(*dimg_heading) + 4, SEEK_SET);
                        fread(mbr_outline, 1, sizeof(*mbr_outline), bin_file);

                        /* Check the data. */
                        FAMP_ASSERT((mbr_outline->MBR_start_ID == FAMP_MBR_START_ID) &&
                                    (strcmp((cpint8) mbr_outline->MBR_start_ID_name, "FENT") == 0) &&
                                    (mbr_outline->MBR_ptbl_entry_ID == FAMP_MBR_PTBL_ENTRIES_ID) &&
                                    (strcmp((cpint8) mbr_outline->MBR_ptbl_entry_ID_name, "FMPT") == 0),
                            "\nInvalid MBR start ID in section found.\n")

                        delete mbr_outline;

                        /* check the entries. */
                        pentry = new struct partition_entry;
                        struct partition_entry prev_entry;
                        
                        {
                            /* Entry 1 - Second Stage Bootloader. */
                            fread(pentry, 1, sizeof(*pentry), bin_file);
                            prev_entry = *pentry;

                            FAMP_ASSERT(pentry->bootable_entry == ENTRY_IS_BOOTABLE,
                                "\nError with first entry in the MBR partition table entry:\n\tThe entry was marked not bootable.\n")
                            FAMP_ASSERT(pentry->entry_type == ENTRY_TYPE_SS,
                                "\nError with first entry in the MBR partition table entry:\n\tThe second stage entry type that gets referenced by the partition entry was not found to be 0x0E.\n")
                            FAMP_ASSERT(pentry->auto_read_program == true,
                                "\nError with first entry in the MBR partition table entry:\n\tThe second stage program is auto read by the MBR partition table C++ program.\n\tIt was found to be set to false in the entry.\n")
                        }

                        {
                            fread(pentry, 1, sizeof(*pentry), bin_file);

                            FILE *fsbin = fopen(fs_bin, "rb");
                            FAMP_ASSERT(fsbin,
                                "\nError opening up the FileSystem (FS) binary whilst checking MBR partition table entries.\n")
                            
                            fseek(fsbin, 0, SEEK_END);
                            size_t fs = ftell(fsbin) / 512;
                            fseek(fsbin, 0, SEEK_SET);
                            fclose(fsbin);

                            FAMP_ASSERT(pentry->bootable_entry != ENTRY_IS_BOOTABLE,
                                "\nError with the second entry in the MBR partition table:\n\tThe second entry describes the FileSystem (FS) and should not be bootable.\n")
                            FAMP_ASSERT(pentry->starting_sector == 0x02,
                                "\nError with the second entry in the MBR partition table:\n\tThe starting sector of the FileSystem (FS) is not aligned with the last sector of the first MBR partition table entry.\n")
                            FAMP_ASSERT(pentry->sector_amnt == fs,
                                "\nError with the second entry in the MBR partition table entry:\n\tThe sector size does not match the sector size of the binary file `%s`.\n",
                                fs_bin)
                            FAMP_ASSERT(pentry->auto_read_program == true,
                                "\nError with the second entry in the MBR partition table entry:\n\tThe FileSystem (FS) is auto read by the MBR partition table C++ program.\n")
                            FAMP_ASSERT(pentry->entry_type == ENTRY_TYPE_FILESYSTEM,
                                "\nError with the second entry in the MBR partition table entry:\n\tThe entry type did was not found to represent the FileSystem (FS), of which the second entry of the MBR partition table\n\tshould represent.\n")
                        }

                        /* The next two entries should be unused. */
                        {
                            uint8 i = 0;

                            reloop:
                            if(i == 2) goto outside;
                            fread(pentry, 1, sizeof(*pentry), bin_file);
                            
                            FAMP_ASSERT(pentry->entry_type == ENTRY_TYPE_UNUSED,
                                "\nError:\n\tOne of the last two MBR partition table entries was not found to be unused, of which the last two entries should be.\n")
                            i++;
                            goto reloop;
                        }

                        outside:
                        delete pentry;

                        disk_image_size += 512;
                        disk_image_pos = 512;
                    }

                    fseek(bin_file, 0, SEEK_SET);
                    
                    /* For some reason, the header signature gets read in wrong. Rever the value. */
                    revert_value<uint32> (dimg_heading->HeaderSig);

                    fread(disk_image_data, 512, sizeof(*disk_image_data), bin_file);
                    fclose(bin_file);

                    return;
                }
                case program::MBR_PART_TABLE: {
                    size_t padding = init_adjustment(mbr_part_table_bin, FAMP_MEM_STAMP_PTBLE_P, false);

                    /* Read in the binary data that will be written before the padding/memory stamp. */
                    size_t file_size = get_file_data();
                    fclose(bin_file);

                    if((file_size + sizeof(mem_stamp) + FAMP_SUBHEADING_SIZE + padding) < 1536)
                        padding = (1536 - file_size) - sizeof(mem_stamp) - FAMP_SUBHEADING_SIZE;
                    
                    {
                        /* Open the binary file to be written to. */
                        bin_file = fopen(mbr_part_table_bin, "wb");

                        write_new_binary(
                            false,
                            *sheading,
                            mem_stamp,
                            file_size,//(disk_image_size - 512) - padding,
                            padding + FAMP_SUBHEADING_SIZE
                        );
                    }

                    /* Get the data into the disk image array. */
                    read_into_disk_image(mbr_part_table_bin);

                    delete padding_val;
                    if(bin_file_data) free(bin_file_data);

                    return;
                }
                case program::FS_WORKER: {
                    size_t padding = init_adjustment(fs_worker_bin, FAMP_MEM_STAMP_FS_WORKER, true);
                    size_t file_size = get_file_data();
                    fclose(bin_file);

                    if((file_size + sizeof(mem_stamp) + FAMP_SUBHEADING_SIZE + padding) < 512)
                        padding = (512 - file_size) - sizeof(mem_stamp) - FAMP_SUBHEADING_SIZE;

                    sheading = new struct FAMP_PROTOCOL_SUBHEADING;

                    init_subheading(
                        *sheading,
                        (file_size + padding + FAMP_SUBHEADING_SIZE + 8),
                        (file_size + padding + FAMP_SUBHEADING_SIZE + 8) / 512,
                        false
                    );

                    {
                        bin_file = fopen(fs_worker_bin, "wb");

                        write_new_binary(
                            true,
                            *sheading,
                            mem_stamp,
                            file_size,
                            padding
                        );
                    }

                    read_into_disk_image(fs_worker_bin);

                    delete sheading;
                    if(padding_val) delete padding_val;
                    if(bin_file_data) free(bin_file_data);
                    return;
                }
                case program::SECOND_STAGE: {
                    size_t padding = init_adjustment(second_stage_bin, FAMP_MEM_STAMP_SECOND_STAGE, true);
                    
                    /* Pad the binary. */
                    size_t file_size = get_file_data();
                    fclose(bin_file);

                    /* Make sure the second stage is multiple of 1024 bytes. */
                    FAMP_ASSERT(file_size < 1024,
                        "\nThe second stage takes up more than 2 sectors (1024 bytes).\n")
                    
                    if((file_size + sizeof(mem_stamp) + FAMP_SUBHEADING_SIZE + padding) < 1024)
                        padding = (1024 - file_size) - sizeof(mem_stamp) - FAMP_SUBHEADING_SIZE;

                    /* Configure the subheading. */
                    sheading = new struct FAMP_PROTOCOL_SUBHEADING;
                    init_subheading(
                        *sheading,
                        (file_size + padding + FAMP_SUBHEADING_SIZE + 8),
                        (file_size + padding + FAMP_SUBHEADING_SIZE + sizeof(mem_stamp)) / 512,
                        false
                    );
                    
                    {
                        /* Reopen the second stage binary file. */
                        bin_file = fopen(second_stage_bin, "wb");

                        write_new_binary(
                            true,
                            *sheading,
                            mem_stamp,
                            file_size,
                            padding
                        );
                    }

                    read_into_disk_image(second_stage_bin);

                    delete sheading;
                    if(padding_val) delete padding_val;
                    if(bin_file_data) free(bin_file_data);

                    return;
                }
                case program::FILESYSTEM: read_into_disk_image(fs_bin);return;break;
                default: FAMP_ERROR("\nUnknown error occurred.\n")
            }
        }

        ~adjust_binary()
        {
            if(disk_image_data) delete disk_image_data;
            
            free((pint8) disk_image_path);
            free((pint8) mbr_binary);
            free((pint8) mbr_part_table_bin);
            free((pint8) fs_worker_bin);
            free((pint8) second_stage_bin);
            free((pint8) fs_bin);
        }
    };
}

#endif