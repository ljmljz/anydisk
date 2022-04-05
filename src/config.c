#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <err.h>

#include "config.h"
#include "inih/ini.h"

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0

static int config_handler(void* user, const char* section, const char* name, const char* value) {
    anydisk_config* pconfig = (anydisk_config*)user;
    char * endptr;

    if (MATCH("ANYDISK", "size")) {
        pconfig->disk_size = strtoull_with_prefix(value, &endptr);
        if (*endptr != '\0') {
            /* failed to parse integer */
            errx(EXIT_FAILURE, "SIZE must be an integer");
        }
    } else if (MATCH("ANYDISK", "type")) {
        pconfig->disk_type = strdup(value);
    } else if (MATCH("ANYDISK", "device")) {
        pconfig->nbd_device = strdup(value);
    } else if (MATCH("ANYDISK", "param")) {
        pconfig->param_file = strdup(value);
    } else if (MATCH("ANYDISK", "output")) {
        pconfig->output_file = strdup(value);
    } else {
        return false;
    }

    return true;
}

static unsigned long long strtoull_with_prefix(const char * str, char * * end) {
  unsigned long long v = strtoull(str, end, 0);
  switch (**end) {
    case 'K':
      v *= 1024;
      *end += 1;
      break;
    case 'M':
      v *= 1024 * 1024;
      *end += 1;
      break;
    case 'G':
      v *= 1024 * 1024 * 1024;
      *end += 1;
      break;
  }
  return v;
}

int load_config(const char* filename, anydisk_config* config) {
    if (ini_parse(filename, config_handler, config) < 0) {
        return false;
    } else {
        return true;
    }
}
