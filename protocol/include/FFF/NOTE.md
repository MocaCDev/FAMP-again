FFF - Famp File Format
    -> Read the FAMP specifcation for more in-depth information about the FFF.

    In Short:
        FAMP_HEADER_START_SIGNATURE - First 4 bytes of any FAMP-related disk image; denotes the value "FAMP" via a 32-bit (4-byte) value.
        FAMP_HEADER_END_SIGNATURE - Last 2 bytes of the FAMP-related disk image header; denotes the value "EE" via a 16-bit (2-byte) value.
        FAMP_CURRENT_REVISION - The current revision of the protocol, this also implies that the version it resembles is the most recent, up to date, and stable version.
        FAMP_MAX_USED_ENTRIES - This signifies how many entries can be used up in the MBR partition table. The MBR partition table only allows 4 entries, so if the value in the header that signifies how many of them are used is > 4, the configuration will error.

        FAMP_SUBHEADER_SIGNATURE - Each program following the initial first sector (first 512 bytes) of the disk image will have a sub heading at the beginning. This will be the first 4 bytes denoting that the following bytes are a part of the sub header; denotes the value "FSHD" via a 32-bit (4-byte) value.

        FAMP_MBR_START_NAME - "Section 1" following the FAMP-related disk image header. This denotes the entry point of the MBR where all the critical code that will be ran resides; denotes "FEND" via a 32-bit (4-byte) value.
        FAMP_MBR_START_ID - The 16-bit (2-byte) ID in relations to FAMP_MBR_START_NAME.

        FAMP_MBR_PTBL_ENTRIES_NAME - "Section 2" following the entry point of the MBR where all critical code resides. This denotes the beginning of the MBR partition table.
        FAMP_MBR_PTBL_ENTRIES_ID - The 16-bit (2-byte) ID in relations to FAMP_MBR_PTBL_ENTRIES_NAME.

        Structures:
            FAMP_PROTOCOL_DISK_IMAGE_HEADING - Header of the FAMP-related disk image.

            FAMP_PROTOCOL_SUBHEADING - Subheading data that can be found at the beginning of each program following the MBR.

            FAMP_SECTION_DATA - Data about each section in relations to assembly program that require critical enforcement of the protocol.

            FAMP_PROTOCOL_MEMORY_STAMP - Data about the memory stamp that is located at the end of each program.
        
        OS_RELATED - Macro that gets defined at the beginning of each OS-related program that tells the file `common.hpp`, located in `include/`, what to do and what not to do.