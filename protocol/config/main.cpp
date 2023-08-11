#include "config_util.hpp"

using namespace YamlParser;
using namespace ConfigFiles;
using namespace ConfigDiskImage;

int main(int args, char *argv[])
{
    FAMP_ASSERT(args > 1,
        "\nMissing arguments for `../bin/config.o`.\n")
    
    yaml_parser *ypars = new yaml_parser("../../boot.yaml");
    ConfigureFiles *config_files = nullptr;

    const auto check_config_files_mem = [&config_files](FileToConfigure ftc)
    {
        if(config_files == nullptr)
            config_files = new ConfigureFiles(ftc);
        else
            config_files->set_new_file_being_configured(ftc);
    };

    if(strcmp((pint8) argv[1], "nm") == 0)
    {
        check_config_files_mem(FileToConfigure::ProtocolMakefile);
        config_files->write_file();
        
        /* Write useres Makefile. */
        config_files->set_new_file_being_configured(FileToConfigure::UserMakefile);
        config_files->write_file();

        goto end;
    }
    if(strcmp((pint8) argv[1], "om") == 0)
    {
        check_config_files_mem(FileToConfigure::OldMakefile);
        config_files->write_file();

        goto end;
    }
    if(strcmp((pint8) argv[1], "mbr") == 0)
    {
        check_config_files_mem(FileToConfigure::MBR);
        config_files->write_file();

        goto end;
    }
    if(strcmp((pint8) argv[1], "dimg") == 0)
    {
        if(config_files)
            delete config_files;
        
        adjust_binary abin(program::MBR);
        abin.adjust();
        abin.switch_binary_program(program::MBR_PART_TABLE);
        abin.adjust();
        abin.switch_binary_program(program::SECOND_STAGE);
        abin.adjust();

        config_image *cimg = dynamic_cast<config_image *> (&abin);
        cimg->write_disk_image();

        goto end;
    }

    end:
    config_files->delete_instance<ConfigureFiles *> (config_files);
    ypars->delete_instance<yaml_parser> (ypars);

    return 0;
}