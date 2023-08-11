#ifndef YAML_PARSER_H
#define YAML_PARSER_H
#include "lexer.hpp"
#include "data.hpp"

using namespace YamlLexer;
using namespace YamlData;

namespace YamlParser
{

struct parser
{
    yaml_lexer *ylexer = nullptr;
    struct token *ytoken = nullptr;

    parser(cpint8 filename)
    {
        ylexer = new yaml_lexer(filename);
        ytoken = ylexer->return_token();
    }

    template<typename T>
        requires std::is_same<T, struct parser>::value
    void delete_instance(T *instance)
    {
        if(instance)
            delete instance;
        instance = nullptr;
    }

    ~parser()
    {
        if(ylexer)
        {
            ylexer->delete_instance<struct token> (ytoken);
            ylexer->delete_instance<yaml_lexer> (ylexer);
        }
    }
};

class yaml_parser
{
private:
    struct parser *yparser = nullptr;
    yaml_data *ydata = nullptr;

    inline void get_token_from_lexer()
    {
        yparser->ylexer->get_token();
        yparser->ytoken = yparser->ylexer->return_token();
    }

    void parse_yaml()
    {
        reloop:
        switch(yparser->ytoken->id)
        {
            case YamlTokens::user_def: {
                puint8 user_defined_var = new uint8[strlen((cpint8) yparser->ytoken->token_value)];
                memcpy(user_defined_var, yparser->ytoken->token_value, strlen((cpint8) yparser->ytoken->token_value));
                
                get_token_from_lexer();
                get_token_from_lexer();

                switch(yparser->ytoken->id)
                {
                    case YamlTokens::hex: {
                        ydata->add_yaml_data(user_defined_var, (puint16) yparser->ytoken->token_value, data_types::Hex);
                
                        break;
                    }
                    case YamlTokens::number: {
                        ydata->add_yaml_data(user_defined_var, (puint16) yparser->ytoken->token_value, data_types::Dec);
                        
                        break;
                    }
                    case YamlTokens::string: {
                        ydata->add_yaml_data(user_defined_var, (puint16) yparser->ytoken->token_value, data_types::Str);
                        break;
                    }
                    case YamlTokens::character: {
                        ydata->add_yaml_data(user_defined_var, (puint16) yparser->ytoken->token_value, data_types::Chr);
                        
                        break;
                    }
                    default: break;
                }

                memset(user_defined_var, 0, strlen((cpint8) user_defined_var));
                delete user_defined_var;
                break;
            }
            case YamlTokens::YamlEOF: {
                goto end;
                break;
            }
            default: break;
        }

        get_token_from_lexer();
        goto reloop;
        
        end:
        return;
    }

    void evaluate_yaml_data()
    {
        yaml_file_data = all_yaml_data;

        while(yaml_file_data->next != nullptr)
        {
            if(yaml_file_data->yod_test((cpint8) "os_type", true))
            {
                if(strcmp((cpint8) yaml_file_data->vdata_as_string, "bit32") == 0)
                    yod.type = 0x02; // 32-bit OS
                else
                    yod.type = 0x03; // 64-bit OS
                
                goto cont;
            }

            if(yaml_file_data->yod_test((cpint8) "os_name", true))
            {
                yod.OS_name = yaml_file_data->vdata_as_string;
                goto cont;
            }

            if(yaml_file_data->yod_test((cpint8) "os_version", true))
            {
                yod.OS_version = yaml_file_data->vdata_as_string;
                goto cont;
            }

            if(yaml_file_data->yod_test((cpint8) "pref_FS", true))
            {
                if(strcmp((cpint8) yaml_file_data->vdata_as_string, "FAMP") == 0)
                { yod.FS_type = (uint8) 1 << 0x04; goto cont; }
                if(strcmp((cpint8) yaml_file_data->vdata_as_string, "FAT32") == 0)
                { yod.FS_type = (uint8) 1 << 0x05; goto cont; }
                if(strcmp((cpint8) yaml_file_data->vdata_as_string, "ISO9660") == 0)
                { yod.FS_type = (uint8) 1 << 0x06; goto cont; }

                yod.FS_type = (uint8) 1 << 0x07;
                goto cont;
            }

            if(yaml_file_data->yod_test((cpint8) "production", true))
            {
                if(strcmp((cpint8) yaml_file_data->vdata_as_string, "yes") == 0)
                { yod.in_production = true; goto cont; }

                yod.in_production = false;
                goto cont;
            }

            if(yaml_file_data->yod_test((cpint8) "disk_name", true))
            {
                yod.disk_name = yaml_file_data->vdata_as_string;
                goto cont;
            }

            if(yaml_file_data->yod_test((cpint8) "auto_format", true))
            {
                if(strcmp((cpint8) yaml_file_data->vdata_as_string, "yes") == 0)
                { yod.auto_format = true; goto cont; }

                yod.auto_format = false; goto cont;
            }

            if(yaml_file_data->yod_test((cpint8) "bin_folder", true))
            {
                yod.bin_folder = yaml_file_data->vdata_as_string;
                goto cont;
            }

            if(yaml_file_data->yod_test((cpint8) "kernel_source", true))
            {
                yod.kernel_source_filename = yaml_file_data->vdata_as_string;
                goto cont;
            }

            if(yaml_file_data->yod_test((cpint8) "kernel_bin", true))
            {
                yod.kernel_bin_filename = yaml_file_data->vdata_as_string;
                goto cont;
            }

            if(yaml_file_data->yod_test((cpint8) "kernel_o", true))
            {
                yod.kernel_o_filename = yaml_file_data->vdata_as_string;
                goto cont;
            }

            cont:
            _next
        }

        /*printf("\n\nYAML OS Data:\n\tOS Type: %X (%s)\n\tOS In Production: %s\n\t",
            yod.type, yod.type == 0x02 ? "bit32" : "bit64",
            yod.in_production ? "Yes" : "No");
        printf("OS Name: %s\n\tOS Version: %s\n\tFS Type: %X (%s)\n\t",
            yod.OS_name, yod.OS_version,
            yod.FS_type, yod.get_fs_name());
        printf("OS Disk Image Name: %s\n\tAuto Format: %s\n\t",
            yod.disk_name, yod.auto_format ? "Yes" : "No");
        printf("Bin Folder: %s\n\tKernel Source Code File: %s\n\t",
            yod.bin_folder, yod.kernel_source_filename);
        printf("Kernel .bin Filename: %s\n\tKernel .o Filename: %s\n",
            yod.kernel_bin_filename, yod.kernel_o_filename);*/
    }

public:
    yaml_parser(cpint8 filename)
    {
        yparser = new struct parser(filename);
        ydata = new yaml_data;

        /* Get the first token. */
        get_token_from_lexer();
        parse_yaml();

        /* Evaluate the data. */
        evaluate_yaml_data();
    }

    template<typename T>
        requires std::is_same<T, struct parser>::value
            || std::is_same<T, yaml_parser>::value
    void delete_instance(T *instance)
    {
        if(instance)
            delete instance;
        instance = nullptr;
    }

    ~yaml_parser()
    {
        if(yparser) yparser->delete_instance<struct parser> (yparser);
        if(ydata) ydata->delete_instance<yaml_data> (ydata);
    }
};

}

#endif
