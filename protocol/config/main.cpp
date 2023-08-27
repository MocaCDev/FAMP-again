#include "config_util.hpp"

int main(int args, char *argv[])
{
    FAMP_ASSERT(args > 1,
        "\nMissing arguments for `../bin/config.o`.\n")
    
    ConfigureFiles &config_files = *new ConfigureFiles(FileToConfigure::None);

    const auto check_config_files_mem = [&config_files](FileToConfigure ftc)
    {
        config_files.set_new_file_being_configured(ftc);
    };

    if(strcmp((pint8) argv[1], "nm") == 0)
    {
        check_config_files_mem(FileToConfigure::ProtocolMakefile);
        config_files.write_file();
        
        /* Write useres Makefile. */
        config_files.set_new_file_being_configured(FileToConfigure::UserMakefile);
        config_files.write_file();

        goto end;
    }
    if(strcmp((pint8) argv[1], "om") == 0)
    {
        check_config_files_mem(FileToConfigure::OldMakefile);
        config_files.write_file();

        goto end;
    }
    if(strcmp((pint8) argv[1], "mbr") == 0)
    {
        check_config_files_mem(FileToConfigure::MBR);
        config_files.write_file();

        goto end;
    }
    if(strcmp((pint8) argv[1], "dimg") == 0)
    {   
        adjust_binary abin(program::MBR);
        abin.adjust();

        abin.switch_binary_program(program::FILESYSTEM);
        abin.adjust();

        abin.switch_binary_program(program::MBR_PART_TABLE);
        abin.adjust();

        abin.switch_binary_program(program::SECOND_STAGE);
        abin.adjust();

        abin.switch_binary_program(program::FS_WORKER);
        abin.adjust();

        config_image *cimg = dynamic_cast<config_image *> (&abin);
        cimg->write_disk_image();

        goto end;
    }

    end:
    return 0;
}