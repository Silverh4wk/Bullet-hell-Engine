#include "global.h"
#include "../io/io.h"
#include "config.h"


static const char CONFIG_DEFAULT[] =
"[Controls]\n"
"down = S\n"
"up = W\n"
"left = A\n"
"right = D\n"
"escape = ESCAPE\n"
"hplus = J\n"
"hminus = L\n"
"vplus = I\n"
"vminus = K\n"
"toggle = P\n";

static char tmpBuffer[20] = {0};

//Internal function to get the configuration entry from a buffer
static char* config_get_value(const char *config_buffer, const char* value) {
    
    const char* line = strstr(config_buffer,value);
    if(!line)
	ERROR_EXIT("Could not find config value %s."
		   "Try deleting 'config.ini' and restarting.\n",value);
    size_t len  = strlen(line);
    const char *end = line+len;
    const char *current = line;
    char *tmpPointer = &tmpBuffer[0];
    
    for(int i =0;i<ArrayCount(tmpBuffer);i++){
	*(tmpPointer+i) = '\0';
    }
    
    //skip the '='.
    while(*current != '=' && current != end)
	++current;
    ++current;
    //skip the spaces/newLines too
    while(*current == ' ')
	++current;
    while(*current != '\n' && *current!= 0 &&current != end )
	*tmpPointer++ = *current++;

    	*(tmpPointer+1) = 0;
    
    
    //finally return pointer to the start of the buffer
    return tmpBuffer;};

// internal function that binds keys
// binds them specifically to what is in the config file and the sdl code and i really do need a better way to do this
static void load_controls(const char *config_buffer)
{
    config_key_bind(INPUT_KEY_DOWN, config_get_value(config_buffer, "down"));
    config_key_bind(INPUT_KEY_UP, config_get_value(config_buffer, "up"));
    config_key_bind(INPUT_KEY_LEFT, config_get_value(config_buffer, "left"));
    config_key_bind(INPUT_KEY_RIGHT, config_get_value(config_buffer, "right"));
    config_key_bind(INPUT_KEY_ESCAPE, config_get_value(config_buffer, "escape"));
    config_key_bind(INPUT_KEY_HPLUS, config_get_value(config_buffer, "hplus"));
    config_key_bind(INPUT_KEY_HMINUS, config_get_value(config_buffer, "hminus"));
    config_key_bind(INPUT_KEY_VPLUS, config_get_value(config_buffer, "vplus"));
    config_key_bind(INPUT_KEY_VMINUS, config_get_value(config_buffer, "vminus"));
    config_key_bind(INPUT_KEY_TOGGLE, config_get_value(config_buffer, "toggle"));
}

//fetch the configuration file and loaded it
static bool32 config_load(void) {
    struct File_S file_config = ioFileRead("./config.ini");
    if(!file_config.is_valid)
	return 0;
    load_controls(file_config.data);
    free(file_config.data);
    return 1;
};

void config_init(void) {
    //attempt to load the config file
    //if fail, we attempt to use default bindings
    if(config_load() == 1)
	return;
    ioFileWrite((void*) CONFIG_DEFAULT, strlen(CONFIG_DEFAULT), "./config.ini");
    //if it still fail to load we exit the program 
    if(config_load() == 0)
	ERROR_EXIT("Could not load the config file. \n")
};

            
void config_key_bind(Input_Key key, const char *key_name) {
    SDL_Scancode scanCode = SDL_GetScancodeFromName(key_name);
    if(scanCode == SDL_SCANCODE_UNKNOWN)
	ERROR_RETURN(,"Invalid scan code while binding key: %s\n" , key_name)
	    global.config.keybinds[key] = scanCode;
};
