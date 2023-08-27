#ifndef FAMP_PROTOCOL_SECOND_STAGE_H
#define FAMP_PROTOCOL_SECOND_STAGE_H
#define OS_RELATED
#define NEED_MBR_PART_TABLE_ADDRESSES
#include <common.hpp>
#include <FFS/FAMP_fs.hpp>
#include <protocol/gdt.h>
#include <bootloader/bootloader_structures.hpp>

using namespace FAMP_FS;
using namespace BootloaderStructs;

extern struct FAMP_PROTOCOL_DISK_IMAGE_HEADING disk_image_heading[];
extern uint8 filesystem[];
extern struct partition_entry pentry[];

extern "C" void __load_gdt();
extern "C" void __prepare_jump_to_pmode();

#endif