typedef struct anydisk_config_t
{
    /* data */
    int disk_size;
    const char* disk_type;
    const char* nbd_device;
    const char* param_file;
    const char* output_file;

} anydisk_config;

static int config_handler(void* user, const char* section, const char* name, const char* value);
static unsigned long long strtoull_with_prefix(const char * str, char * * end);
int load_config(const char* filename, anydisk_config* config);