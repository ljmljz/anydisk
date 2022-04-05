#include "disksim_interface.h"

typedef	double SysTime;		/* system time in seconds.usec */

struct disksim_interface *disksim;

static int anydisk_read(void *buf, u_int32_t len, u_int64_t offset, void *userdata);
static int anydisk_write(const void *buf, u_int32_t len, u_int64_t offset, void *userdata);
static void anydisk_disc(void *userdata);
static int anydisk_flush(void *userdata);
static int anydisk_trim(u_int64_t from, u_int32_t len, void *userdata);

void syssim_schedule_callback(disksim_interface_callback_t fn, SysTime t, void *ctx);
void syssim_deschedule_callback(double t, void *ctx);
void syssim_report_completion(SysTime t, struct disksim_request *r, void *ctx);