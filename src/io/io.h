#pragma once
#include "../helpers.h"

typedef struct File {
    char *data;
    size_t len;
    bool32 is_valid;
	
} File;

File ioFileRead(const char* path);
int ioFileWrite(void* buffer, size_t size, const char* path);
