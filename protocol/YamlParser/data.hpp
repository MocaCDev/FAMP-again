#ifndef YAML_DATA_H
#define YAML_DATA_H

namespace YamlData
{

struct yaml_os_data
{
    uint8        type;
    bool         in_production;

    /* General OS information. */
    puint8       OS_name;
    puint8       OS_version;
    uint8        FS_type;

    /* Disk image name. */
    puint8       disk_name;

    /* Does the user want FAMP to auto format the disk image? */
    bool         auto_format;

    /* Binary folder. */
    puint8       bin_folder;

    /* Kernel info. */
    puint8       kernel_source_filename;
    puint8       kernel_bin_filename;
    puint8       kernel_o_filename;

    puint8 get_fs_name()
    {
        switch(FS_type)
        {
            case 1 << 0x04: return (puint8) "FAMP";break;
            case 1 << 0x05: return (puint8) "FAT32";break;
            case 1 << 0x06: return (puint8) "ISO9660";break;
            case 1 << 0x07: return (puint8) "EXT";break;
            default: break;
        }
        return (puint8) "Unknown FS";
    }
};

static struct yaml_os_data yod;

enum class data_types
{
    Chr,
    Hex,
    Dec,
    Str
};

struct data
{
    puint8        user_defined;
    data_types    data_type;
    puint16       vdata;
    puint8        vdata_as_string;

    struct data   *next;
    struct data   *previous;

    data()
    {
        user_defined = nullptr;
        vdata = nullptr;
        next = nullptr;
        previous = nullptr;
        vdata_as_string = nullptr;
    }

    void yaml_get_data_as_string()
    {
        if(vdata_as_string)
        {
            memset(vdata_as_string, 0, strlen((cpint8) vdata_as_string));
            free(vdata_as_string);
            vdata_as_string = nullptr;
        }
        vdata_as_string = (puint8) calloc(1, sizeof(*vdata_as_string));
        uint8 index = 0;
        uint8 DAS_index = 0;

        while(vdata[index])
        {
            vdata_as_string[DAS_index] = vdata[index] & 0xFF;
            DAS_index++;
            vdata_as_string = (puint8) realloc(
                vdata_as_string,
                (DAS_index + 1) * sizeof(*vdata_as_string)
            );
            
            vdata_as_string[DAS_index] = (vdata[index] >> 8) & 0xFF;
            DAS_index++;
            vdata_as_string = (puint8) realloc(
                vdata_as_string,
                (DAS_index + 1) * sizeof(*vdata_as_string)
            );
            index++;
        }
    }

    inline uint8 yaml_get_data_as_byte()
    {
        return vdata[0] & 0xFF;
    }

    inline bool yod_test(cpint8 if_is, bool must_be_string)
    {
        if(strcmp((cpint8) user_defined, if_is) == 0)
        {
            if(must_be_string)
            {
                yaml_get_data_as_string();
                return true;
            }
            yaml_get_data_as_byte();
            return true;
        }
        return false;
    }

    template<typename T>
        requires std::is_same<T, struct data>::value
    void delete_instance(T *instance)
    {
        if(instance)
            delete instance;
        instance = nullptr;
    }

    ~data()
    {
        if(user_defined) delete user_defined;
        user_defined = nullptr;

        if(vdata) delete vdata;
        vdata = nullptr;

        if(vdata_as_string) free(vdata_as_string);
        vdata_as_string = nullptr;
    }
};

static struct data *yaml_file_data = nullptr;
static struct data *all_yaml_data = nullptr;
static usize yaml_file_data_size = 0;

#define _next yaml_file_data = yaml_file_data->next;
#define _back yaml_file_data = yaml_file_data->previous;

class yaml_data
{
public:
    yaml_data()
    {
        yaml_file_data = new struct data;
        all_yaml_data = yaml_file_data;
    }

    void add_yaml_data(puint8 user_def_name, puint16 data, data_types type)
    {
        struct data previous = *yaml_file_data;
        yaml_file_data->next = new struct data;

        yaml_file_data->user_defined = new uint8[strlen((cpint8) user_def_name)];
        memcpy(yaml_file_data->user_defined, user_def_name, strlen((cpint8) user_def_name));
        yaml_file_data->user_defined[strlen((cpint8) user_def_name)] = 0;

        yaml_file_data->data_type = type;
        switch(yaml_file_data->data_type)
        {
            case data_types::Chr: yaml_file_data->vdata = (puint16) calloc(1, sizeof(*yaml_file_data->vdata));break;
            case data_types::Dec:
            case data_types::Hex: yaml_file_data->vdata = (puint16) calloc(2, sizeof(*yaml_file_data->vdata));break;
            case data_types::Str: {
                yaml_file_data->vdata = (puint16) calloc(1, sizeof(*yaml_file_data->vdata));
                uint8 index = 0;

                while(data[index])
                {
                    yaml_file_data->vdata[index] = data[index];
                    index++;

                    if(data[index])
                        yaml_file_data->vdata = (puint16) realloc(
                            yaml_file_data->vdata,
                            (index + 1) * sizeof(*yaml_file_data->vdata)
                        );
                }
                goto rest;
                break;
            }
            default: break;
        }
        yaml_file_data->vdata = data;

        rest:
        /* Move on. */
        yaml_file_data = yaml_file_data->next;
        yaml_file_data->next = nullptr;

        /* Previous data. */
        yaml_file_data->previous = new struct data;
        memcpy((void *)yaml_file_data->previous, &previous, sizeof(*yaml_file_data->previous));
        yaml_file_data_size++;
    }

    template<typename T>
        requires std::is_same<T, struct data>::value
            || std::is_same<T, yaml_data>::value
    void delete_instance(T *instance)
    {
        if(instance)
            delete instance;
        instance = nullptr;
    }

    ~yaml_data()
    {
        yaml_file_data = all_yaml_data;

        while(yaml_file_data->next)
        {
            yaml_file_data->delete_instance<struct data> (yaml_file_data);
            yaml_file_data = yaml_file_data->next;
            delete yaml_file_data->previous;
        }

        if(yaml_file_data)
            yaml_file_data->delete_instance<struct data> (yaml_file_data);
        yaml_file_data = nullptr;
        all_yaml_data = nullptr;
    }
};
    
}

#endif
