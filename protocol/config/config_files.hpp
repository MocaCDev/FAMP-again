#ifndef FAMP_CONFIG_FILES_H
#define FAMP_CONFIG_FILES_H

using namespace YamlParser;

namespace ConfigFiles
{
    cpint8 mbr_format = (cpint8) initiate_path((pint8)"formats/", (pint8)"test_mbr");
    cpint8 protocol_makefile = (cpint8) initiate_path((pint8)"formats/", (pint8)"protocol_mf");
    cpint8 user_makefile = (cpint8) initiate_path((pint8)"formats/", (pint8)"user_mf");
    cpint8 old_mf = (cpint8) initiate_path((pint8)"formats/", (pint8)"old_mf");
    cpint8 fs_bin = (cpint8) initiate_path((pint8)"../bin/", (pint8)"fs.bin");
    
    enum class FileToConfigure: uint8_t
    {
        UserMakefile,
        ProtocolMakefile,
        MBR,
        OldMakefile,
        None
    };

    class ConfigureFiles
    {
    private:
        FILE *config_file;
        FileToConfigure file_being_configured;

        /* Needed to get the yaml file data. */
        yaml_parser &ypars = *new yaml_parser("../../boot.yaml");

        inline puint8 get_file_data()
        {
            FAMP_ASSERT(config_file,
                "\nAn error occurred with the format file that was opened prior. It is this, or the file was not found.\n")
            
            fseek(config_file, 0, SEEK_END);
            size_t format_size = ftell(config_file);
            fseek(config_file, 0, SEEK_SET);

            FAMP_ASSERT(format_size > 1,
                "\nThere was no data found in the format file.\n")
            
            puint8 format = new uint8[format_size];
            FAMP_ASSERT(format,
                "\nError allocating new memory for reading in the format.\n")
            
            FAMP_ASSERT(fread(format, sizeof(uint8), format_size, config_file) == format_size,
                "\nError reading in the needed %ld bytes.\n",
                format_size)
            
            return format;
        }
    
    public:
        ConfigureFiles(FileToConfigure ftc)
            : config_file(nullptr)
        {
            set_new_file_being_configured(ftc);
        }

        inline void set_new_file_being_configured(FileToConfigure ftc)
        {
            file_being_configured = ftc;

            if(config_file)
                fclose(config_file);
            
            switch(ftc)
            {
                case FileToConfigure::None: break;
                case FileToConfigure::UserMakefile: config_file = fopen(user_makefile, "r");break;
                case FileToConfigure::ProtocolMakefile: config_file = fopen(protocol_makefile, "r");break;
                case FileToConfigure::MBR: config_file = fopen(mbr_format, "r");break;
                case FileToConfigure::OldMakefile: config_file = fopen(old_mf, "r");break;
                default: FAMP_ERROR("\nUnexpected argument passed to `ConfigureFiles` class constructor.\n")
            }
        }

        void write_file()
        {
            FILE *source_file = nullptr;
            bool needs_to_format = false;
            
            /* Based on `file_being_configured`, open the according source file. */
            switch(file_being_configured)
            {
                case FileToConfigure::UserMakefile: source_file = fopen("../../Makefile", "w");break;
                case FileToConfigure::ProtocolMakefile: needs_to_format = true;source_file = fopen("../Makefile", "w");break;
                case FileToConfigure::MBR: needs_to_format = true;source_file = fopen("../boot/mbr.s", "w");break;
                case FileToConfigure::OldMakefile: source_file = fopen("../Makefile", "w");break;
                default: FAMP_ERROR("\nAn unknown error occurred.\n")
            }

            puint8 format = get_file_data();
            
            if(needs_to_format)
            {
                uint8 completed_format[strlen((cpint8) format) + 120];
                memset(completed_format, 0, strlen((cpint8) format) + 120);

                if(file_being_configured == FileToConfigure::ProtocolMakefile)
                {
                    sprintf((pint8) completed_format, (cpint8) format,
                        yod.kernel_bin_filename, yod.kernel_bin_filename, 
                        yod.kernel_bin_filename, yod.kernel_o_filename,
                        yod.kernel_bin_filename,
                        yod.kernel_o_filename, yod.kernel_source_filename,
                        yod.kernel_o_filename, yod.kernel_bin_filename,
                        yod.bin_folder, yod.bin_folder
                    );
                    
                    goto write;
                }

                if(file_being_configured == FileToConfigure::MBR)
                {
                    /* Formatting FS binary to be multiple of 512 bytes. */
                    {
                        FILE *fs_binary;

                        fs_binary = fopen(fs_bin, "rb");
                        FAMP_ASSERT(fs_binary,
                            "\nError opening up the fs binary file `%s`.\n",
                            fs_bin)
                        
                        fseek(fs_binary, 0, SEEK_END);
                        size_t fs_bin_size = ftell(fs_binary);
                        fseek(fs_binary, 0, SEEK_SET);

                        size_t bytes_needed = fs_bin_size;
                        while(bytes_needed % 512 != 0)
                            bytes_needed++;
                        
                        uint8 padding[bytes_needed - fs_bin_size];
                        memset(padding, 0, bytes_needed - fs_bin_size);
                        puint8 fs_data = new uint8[fs_bin_size];
                        fread(fs_data, fs_bin_size, sizeof(*fs_data), fs_binary);

                        fclose(fs_binary);

                        {
                            fs_binary = fopen(fs_bin, "wb");
                            fwrite(fs_data, fs_bin_size, sizeof(*fs_data), fs_binary);
                            fwrite(&padding, bytes_needed - fs_bin_size, sizeof(uint8), fs_binary);
                            fclose(fs_binary);
                        }

                        delete fs_data;
                    }

                    /* Get the filesystem binary size. */
                    FILE *fbin = fopen(fs_bin, "rb");
                    FAMP_ASSERT(fbin,
                        "\nError opening up `%s`.\n", fs_bin)
                    
                    fseek(fbin, 0, SEEK_END);
                    size_t fbin_size = ftell(fbin);
                    fseek(fbin, 0, SEEK_SET);
                    fclose(fbin);

                    uint8 os_name[14];
                    memset(os_name, ' ', 14);
                    memcpy(os_name, yod.OS_name, strlen((cpint8) yod.OS_name));

                    sprintf((pint8) completed_format, (cpint8) format,
                        os_name, yod.type, yod.OS_version,
                        yod.FS_type, yod.in_production,
                        fbin_size / 512,
                        0x2 + (fbin_size / 512),
                        0x2 + (fbin_size / 512) + 0x3,
                        (0x5 + (fbin_size / 512)) + 0x2,
                        (0x5 + (fbin_size / 512)) + 0x2,
                        (0x5 + (fbin_size / 512)) + 0x2 + (fbin_size / 512),
                        (fbin_size / 512));

                    goto write;
                }
                
                write:
                fwrite(completed_format, sizeof(uint8), strlen((cpint8) completed_format), source_file);
                goto end;
            }

            fwrite(format, sizeof(uint8), strlen((cpint8) format), source_file);
            
            end:
            fclose(source_file);
            delete format;
        }

        ~ConfigureFiles()
        {
            if(config_file) fclose(config_file);
            config_file = nullptr;
        }
    };
}

#endif