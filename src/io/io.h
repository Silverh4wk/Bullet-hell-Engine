#ifndef IO_H
#define IO_H

#include "../helpers.h"

struct File_S {
    char *data;
    size_t len;
    bool32 is_valid;
	
};

//read the file content and return it inside the File_S struct 
struct File_S
ioFileRead(const char* path);

//Write to a file
int
ioFileWrite(void* buffer, size_t size, const char* path);

#endif
