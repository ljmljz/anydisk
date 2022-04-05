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

static anydisk_config disk_config;
/* BUSE callbacks */
static void *data;
static SysTime now = 0;		/* current time */
static SysTime next_event = -1;	/* next event */
static int completed = 0;	/* last request was completed */

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

/*
 * Schedule next callback at time t.
 * Note that there is only *one* outstanding callback at any given time.
 * The callback is for the earliest event.
 */
void syssim_schedule_callback(disksim_interface_callback_t fn, SysTime t, void *ctx)
{
  next_event = t;
}

/*
 * de-scehdule a callback.
 */
void syssim_deschedule_callback(double t, void *ctx)
{
  next_event = -1;
}


void syssim_report_completion(SysTime t, struct disksim_request *r, void *ctx)
{
  completed = 1;
  now = t;
  // add_statistics(&st, t - r->start);
}


int main(int argc, char *argv[]) {
  int verbose = 0;

  load_config(argv[1], &disk_config);

  struct buse_operations aop = {
    .read = anydisk_read,
    .write = anydisk_write,
    .disc = anydisk_disc,
    .flush = anydisk_flush,
    .trim = anydisk_trim,
    .size = disk_config.disk_size,
  };

  data = malloc(aop.size);
  if (data == NULL) err(EXIT_FAILURE, "failed to alloc space for data");

  disksim = disksim_interface_initialize(
    disk_config.param_file, 
    disk_config.output_file,
    syssim_report_completion,
    syssim_schedule_callback,
    syssim_deschedule_callback,
    0,
    0,
    0
  );

  /* NOTE: it is bad to use this internal disksim call from external... */
  DISKSIM_srand48(1);

  return buse_main(disk_config.nbd_device, &aop, (void *)&verbose);
}
