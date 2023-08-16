#ifndef FAMP_PROTOCOL_MBR_PART_TABLE_H
#define FAMP_PROTOCOL_MBR_PART_TABLE_H
#define OS_RELATED
#include <common.hpp>
#include <FFF/FFF_structures.hpp>
#include <bootloader/bootloader_structures.hpp>

#define SECOND_STAGE_ADDRESS        0x7E00

using namespace FFF_Structures;
using namespace BootloaderStructs;

struct FAMP_PROTOCOL_DISK_IMAGE_HEADING *heading = (struct FAMP_PROTOCOL_DISK_IMAGE_HEADING *) 0x0B00;
struct MBR_bin_outline *entry_code = (struct MBR_bin_outline *) 0x0C84;

void read_in_memory(uint16 addr, uint8 start_sector, uint8 sector_amount);

extern void hi(void);
#endif