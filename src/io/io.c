#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "io.h"

//size is set to 20MiB
#define IO_READ_CHUNK_SIZE MegaBytes(20)
#define IO_READ_ERROR_GENERAL "Error reading file: %s. errno: %d\n"
#define IO_WRITE_ERROR_GENERAL "Error writing into file: %s. errno: %d\n"
#define IO_READ_ERROR_MEMORY  "Not enough memory to read the fwssile: %s\n"

struct File_S ioFileRead(const char *path)
{
    struct File_S file;
    file.is_valid = false  ;
    FILE *filePointer = fopen(path,"rb");
    if(!filePointer||ferror(filePointer)){
	ERROR_RETURN(file,IO_READ_ERROR_GENERAL, path, errno);
    }
    char *data = NULL;
    char *temp;
    // to figure out how many bytes we used
    size_t used = 0;
    size_t size = 0;
    size_t n = 0;

    //read the file content
    while(true){
	//Expands the buffer when needed
	if(used + IO_READ_CHUNK_SIZE + 1 > size)
	{
	    size = used + IO_READ_CHUNK_SIZE + 1;
	    //overflow check
	    if(size<=used){
		free(data);
		ERROR_RETURN(file, IO_READ_ERROR_MEMORY,path);
	    }

	    temp =(char*) realloc(data,size);
	    if(!temp)
	    {
		free(temp);
		ERROR_RETURN(file,IO_READ_ERROR_GENERAL,path,errno);
	    }
	    data = temp;
	}
	
	n =  fread(data+used, 1, IO_READ_CHUNK_SIZE,filePointer);
	if(n==0)
	    break;
	used +=n ;
    }
    
    if(ferror(filePointer)){
	free(data);
	ERROR_RETURN(file,IO_READ_ERROR_GENERAL,path,errno);
    }

    //resizing the data buffer to the correct size
    temp =(char*) realloc(data, used+1);
    if(!temp)
    {
	free(temp);
	ERROR_RETURN(file,IO_READ_ERROR_GENERAL,path,errno);
    }
    data=temp;
    //after resizing the data buffer to the correct size
    //add a null terminator to the end
    data[used] = 0;
    file.data=data;
    file.len = used;
    file.is_valid = true;
    return file;
};

int ioFileWrite(void *buffer, size_t size, const char *path)
{
    FILE *filePointer = fopen(path,"wb");
    if(!filePointer|| ferror(filePointer)){
	ERROR_RETURN(1,IO_WRITE_ERROR_GENERAL, path,errno)
	    }
    size_t chunks_written =  fwrite(buffer,size,1,filePointer);
    fclose(filePointer);
    if(chunks_written != 1)
	ERROR_RETURN(1,"WRITE ERROR. "
		        "Expected 1 chunk...., got %zu \n", chunks_written);
    return 0;
}
