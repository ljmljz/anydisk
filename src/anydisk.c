/*
 * busexmp - example memory-based block device using BUSE
 * Copyright (C) 2013 Adam Cozzette
 *
 * This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <argp.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "anydisk.h"
#include "config.h"
#include "buse.h"

static anydisk_config configuration;
/* BUSE callbacks */
static void *data;

static int anydisk_read(void *buf, u_int32_t len, u_int64_t offset, void *userdata)
{
  if (*(int *)userdata)
    fprintf(stderr, "R - %lu, %u\n", offset, len);
  memcpy(buf, (char *)data + offset, len);
  return 0;
}

static int anydisk_write(const void *buf, u_int32_t len, u_int64_t offset, void *userdata)
{
  if (*(int *)userdata)
    fprintf(stderr, "W - %lu, %u\n", offset, len);
  memcpy((char *)data + offset, buf, len);
  return 0;
}

static void anydisk_disc(void *userdata)
{
  if (*(int *)userdata)
    fprintf(stderr, "Received a disconnect request.\n");
}

static int anydisk_flush(void *userdata)
{
  if (*(int *)userdata)
    fprintf(stderr, "Received a flush request.\n");
  return 0;
}

static int anydisk_trim(u_int64_t from, u_int32_t len, void *userdata)
{
  if (*(int *)userdata)
    fprintf(stderr, "T - %lu, %u\n", from, len);
  return 0;
}

/* argument parsing using argp */

static struct argp_option options[] = {
  {"verbose", 'v', 0, 0, "Produce verbose output", 0},
  {0},
};

struct arguments {
  unsigned long long size;
  char * device;
  int verbose;
};

static struct argp argp = {
  .options = options,
  // .parser = parse_opt,
  .args_doc = "SIZE DEVICE",
  .doc = "BUSE virtual block device that stores its content in memory.\n"
         "`SIZE` accepts suffixes K, M, G. `DEVICE` is path to block device, for example \"/dev/nbd0\".",
};


int main(int argc, char *argv[]) {
  struct arguments arguments = {
    .verbose = 0,
  };
  // argp_parse(&argp, argc, argv, 0, 0, &arguments);
  load_config(argv[1], &configuration);

  struct buse_operations aop = {
    .read = anydisk_read,
    .write = anydisk_write,
    .disc = anydisk_disc,
    .flush = anydisk_flush,
    .trim = anydisk_trim,
    .size = arguments.size,
  };

  data = malloc(aop.size);
  if (data == NULL) err(EXIT_FAILURE, "failed to alloc space for data");

  return buse_main(arguments.device, &aop, (void *)&arguments.verbose);
}
