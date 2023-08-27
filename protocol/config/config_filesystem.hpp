#ifndef FAMP_PROTOCOL_CONFIG_FILESYSTEM_H
#define FAMP_PROTOCOL_CONFIG_FILESYSTEM_H
#include <FFS/FAMP_fs.hpp>

using namespace FAMP_FS;
using namespace ConfigDiskImage;

namespace ConfigFilesystem
{
    //uint8 kernel_bin_path[40] = "../../%s";

    /* `kernel_bin_path` is the "formatter" that `abs_kernel_bin_path`
     * will be formatted with.
     * */
    //uint8 abs_kernel_bin_path[40];

    cpint8 kernel_linker_file = (cpint8) initiate_path((pint8) "../linker/", (pint8)"kernel.ld");
    cpint8 kernel_linker_format = (cpint8) initiate_path((pint8) "formats/", (pint8)"kernel_linker_format");

    constexpr uint32 initial_fs_size = 1024;

    class config_famp_fs
    {
    private:
        struct FAMP_FS_HEADER *famp_fs_header;
        struct FAMP_FS_PARTITION_METADATA *famp_fs_metadata;
        FILE *kernel_binary;

        /* Needed for `yod` to get filled out. */
        //yaml_parser &ypars = *new yaml_parser("../../boot.yaml");

    public:
        config_famp_fs()
            : famp_fs_header(nullptr), famp_fs_metadata(nullptr), kernel_binary(nullptr)
        {
            memset(abs_kernel_bin_path, 0, 40);

            sprintf((pint8) abs_kernel_bin_path, (cpint8)kernel_bin_path,
                yod.kernel_bin_filename);

            kernel_binary = fopen((cpint8) abs_kernel_bin_path, "rb");
            FAMP_ASSERT(kernel_binary,
                "\nError opening up the kernel binary file `%s`.\n",
                abs_kernel_bin_path)
        }

        void init_filesystem()
        {
            famp_fs_header = new struct FAMP_FS_HEADER;
            famp_fs_metadata = new struct FAMP_FS_PARTITION_METADATA;

            for(uint8 i = 0; i < 6; i++)
                memset(&famp_fs_header->FampFS_Sig[i], FAMP_FS_HEADER_SIG[i], 1);
            
            famp_fs_header->FampFS_Rev = FAMP_FS_REVISION;
            famp_fs_header->FampFS_NOP = 1; /* the filesystem gets initialized with one partition (the kernel). */
            famp_fs_header->FampFS_BeginSig = revert_value<uint32> (FAMP_FS_BEGIN_SIG);

            /* Initial first partitions metadata (for the kernel). */
            memset(famp_fs_metadata->PartitionName, 0, 100);
            for(uint8 i = 0; i < 6; i++)
                memset(&famp_fs_metadata->PartitionName[i], FAMP_FS_KERNEL_PART_NAME[i], 1);
            
            famp_fs_metadata->PartitionAcc = (uint8) PartitionAccess::ProtocolOnly;
            famp_fs_metadata->PartitionAct = (uint8) PartitionAction::Nothing;
            famp_fs_metadata->PartitionAddressT = (uint8) PartitionAddressType::Virtual;
            famp_fs_metadata->PartitionT = (uint8) PartitionType::Code;
            famp_fs_metadata->PartitionActionCount = (uint16) 0x0;
            famp_fs_metadata->PartitionActionID = (uint8) PartitionIdentity::Protocol;
            famp_fs_metadata->PartitionDataOffset = (uint32) 0x0; /* the kernel offset is zero from the partition metadata (the kernel has no additional information after the metadata of the partition). */
            famp_fs_metadata->PartitionAddress = FAMP_FS_KERNEL_VIRTUAL_ADDRESS;
            famp_fs_metadata->NeedsRelocated = false;
            famp_fs_metadata->PartitionBeginSig = (uint32) revert_value<uint32> (FAMP_FS_PARTITION_BEGIN_SIG);
        }

        void config_kernel_partition()
        {
            const auto get_kernel_bin_size = [this] ()
            {
                fseek(kernel_binary, 0, SEEK_END);
                size_t size = ftell(kernel_binary);
                fseek(kernel_binary, 0, SEEK_SET);

                return size;
            };

            /* Get the size of the kernels binary to make sure it is a multiple of 512 bytes. */
            size_t kernel_bin_size = get_kernel_bin_size();

            size_t bytes_needed = kernel_bin_size;
            while(bytes_needed % 512 != 0)
                bytes_needed++;
            
            uint8 padding[bytes_needed - kernel_bin_size];
            memset(padding, 0, bytes_needed - kernel_bin_size);

            uint8 *kernel_data = new uint8[kernel_bin_size];
            fread(kernel_data, kernel_bin_size, sizeof(uint8), kernel_binary);

            fclose(kernel_binary);
            
            {
                kernel_binary = fopen((cpint8) abs_kernel_bin_path, "wb");
                fwrite(kernel_data, kernel_bin_size, sizeof(*kernel_data), kernel_binary);
                fwrite(&padding, bytes_needed - kernel_bin_size, sizeof(uint8), kernel_binary);

                delete kernel_data;
                fclose(kernel_binary);
            }

            kernel_binary = fopen((cpint8) abs_kernel_bin_path, "rb");
            kernel_bin_size = get_kernel_bin_size();
            kernel_data = new uint8[kernel_bin_size];
            fread(kernel_data, kernel_bin_size, sizeof(*kernel_data), kernel_binary);
            fclose(kernel_binary);

            famp_fs_metadata->PartitionSectorSize = (uint32) kernel_bin_size / 512;
            famp_fs_metadata->PartitionByteSize = (uint32) kernel_bin_size;

            FILE *filesystem_binary = fopen("../bin/fs.bin", "wb");
            fwrite(famp_fs_header, 1, sizeof(*famp_fs_header), filesystem_binary);
            fwrite(famp_fs_metadata, 1, sizeof(*famp_fs_metadata), filesystem_binary);
            fwrite(kernel_data, kernel_bin_size, sizeof(*kernel_data), filesystem_binary);
            fclose(filesystem_binary);

            delete kernel_data;

            uint8 format[500];
            memset(format, 0, 500);

            FILE *fformat = fopen(kernel_linker_format, "rb");
            FAMP_ASSERT(fformat,
                "\nError opening `%s`\n", kernel_linker_format)
            
            fseek(fformat, 0, SEEK_END);
            size_t size = ftell(fformat);
            fseek(fformat, 0, SEEK_SET);

            puint8 raw_format = new uint8[size];
            fread(raw_format, size, sizeof(*raw_format), fformat);

            fclose(fformat);

            sprintf((pint8) format, (cpint8) raw_format,
                FAMP_FS_KERNEL_VIRTUAL_ADDRESS + (initial_fs_size + kernel_bin_size));
            fformat = fopen(kernel_linker_file, "wb");
            fwrite(format, strlen((pint8) format), sizeof(*raw_format), fformat);
            fclose(fformat);
            delete raw_format;
        }

        ~config_famp_fs()
        {
            if(famp_fs_header) delete famp_fs_header;
            if(famp_fs_metadata) delete famp_fs_metadata;
            if(kernel_binary) fclose(kernel_binary);
        }
    };
}

#endif