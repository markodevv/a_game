#include <stdio.h>
#include "data_desk.h"

static FILE *global_header_file = 0;

DATA_DESK_FUNC void
DataDeskCustomInitCallback(void)
{
	global_header_file = fopen("generated.h", "w");
}

DATA_DESK_FUNC void
DataDeskCustomParseCallback(DataDeskNode *root, char *filename)
{
	DataDeskFWriteGraphAsC(global_header_file, root);
}

DATA_DESK_FUNC void
DataDeskCustomCleanUpCallback(void)
{
	fclose(global_header_file);
}
