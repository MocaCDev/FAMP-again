#ifndef FAMP_CONFIG_UTIL_H
#define FAMP_CONFIG_UTIL_H
#include "../YamlParser/parser.hpp"
#include <common.hpp>

pint8 initiate_path(int8 data1[], int8 data2[])
{
	/* `data1`, at least, has to be valid and not NULL. */
	FAMP_ASSERT(data1, "Cannot initiate the path. No data given to configure the path.\n")

	/* Initiate `array` and concat `data1` to it. */
	int8 *array = (int8 *) calloc(strlen(data1), sizeof(*array));
	strcat(array, data1);

	/* If `data2` is not NULL, go ahead and perform the same action as above. */
	if(data2 != NULL)
	{
		array = (int8 *) realloc(array, ((strlen(data1) + 1) + strlen(data2)) * sizeof(*array));
		
		FAMP_ASSERT(strlen(data1) + strlen(data2) <= 80, 
			"Path is too large: %s.\nFAMP only allows up to 50 characters for a path.\n", strcat(array, data2))
		
		strcat(array, data2);
	}

	return array;
}

#include "config_files.hpp"
#include "config_disk_image.hpp"
#include "config_filesystem.hpp"

using namespace ConfigFiles;
using namespace ConfigDiskImage;
using namespace ConfigFilesystem;

#endif