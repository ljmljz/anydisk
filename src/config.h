typedef struct anydisk_config_t
{
    /* data */
    int disk_size;
    const char* disk_type;
    const char* param_file;
    const char* output_file;

} anydisk_config;

int config_handler(void* user, const char* section, const char* name, const char* value);
int load_config(const char* filename, anydisk_config* config);