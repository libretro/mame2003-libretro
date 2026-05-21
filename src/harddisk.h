/***************************************************************************

	Generic MAME hard disk implementation, with differencing files

***************************************************************************/

#ifndef HARDDISK_H
#define HARDDISK_H

#include <chd.h>
#include "driver.h"

struct hard_disk_file;

struct hard_disk_info
{
	uint32_t			cylinders;
	uint32_t			heads;
	uint32_t			sectors;
	uint32_t			sectorbytes;
};



/*************************************
 *
 *	Prototypes
 *
 *************************************/

struct hard_disk_file *hard_disk_open(struct chd_file *chd);
void hard_disk_close(struct hard_disk_file *file);

struct chd_file *hard_disk_get_chd(struct hard_disk_file *file);
struct hard_disk_info *hard_disk_get_info(struct hard_disk_file *file);

uint32_t hard_disk_read(struct hard_disk_file *file, uint32_t lbasector, uint32_t numsectors, void *buffer);
uint32_t hard_disk_write(struct hard_disk_file *file, uint32_t lbasector, uint32_t numsectors, const void *buffer);

#endif /* HARDDISK_H */
