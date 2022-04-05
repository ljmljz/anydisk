#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "config.h"
#include "inih/ini.h"

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0

int config_handler(void* user, const char* section, const char* name, const char* value) {
    anydisk_config* pconfig = (anydisk_config*)user;

    if (MATCH("ANYDISK", "size")) {
        pconfig->disk_size = atoi(value);
    } else if (MATCH("ANYDISK", "type")) {
        pconfig->disk_type = strdup(value);
    } else if (MATCH("ANYDISK", "param")) {
        pconfig->param_file = strdup(value);
    } else if (MATCH("ANYDISK", "output")) {
        pconfig->output_file = strdup(value);
    } else {
        return false;
    }

    return true;
}

int load_config(const char* filename, anydisk_config* config) {
    if (ini_parse(filename, config_handler, config) < 0) {
        return false;
    } else {
        return true;
    }
}
