/***************************************************************************

	Generic MAME hard disk implementation, with differencing files

***************************************************************************/

#include "harddisk.h"



/*************************************
 *
 *	Type definitions
 *
 *************************************/

struct hard_disk_file
{
	struct chd_file *	chd;				/* CHD file */
	struct hard_disk_info info;				/* hard disk info */
	uint32_t				hunksectors;		/* sectors per hunk */
	uint32_t				cachehunk;			/* which hunk is cached */
	uint8_t *				cache;				/* cache of the current hunk */
};



/*************************************
 *
 *	Open a hard disk
 *
 *************************************/

struct hard_disk_file *hard_disk_open(struct chd_file *chd)
{
	int cylinders, heads, sectors, sectorbytes;
	struct hard_disk_file *file;
	char metadata[256];
	uint32_t metatag;
	uint32_t count;

	/* punt if no CHD */
	if (!chd)
		return NULL;
	
	/* read the hard disk metadata */
	metatag = HARD_DISK_STANDARD_METADATA;
	count = chd_get_metadata(chd, &metatag, 0, metadata, sizeof(metadata));
	if (count == 0)
		return NULL;
	
	/* parse the metadata */
	if (sscanf(metadata, HARD_DISK_METADATA_FORMAT, &cylinders, &heads, &sectors, &sectorbytes) != 4)
		return NULL;
	
	/* allocate memory for the hard disk file */
	file = malloc(sizeof(struct hard_disk_file));
	if (!file)
		return NULL;
	
	/* fill in the data */
	file->chd = chd;
	file->info.cylinders = cylinders;
	file->info.heads = heads;
	file->info.sectors = sectors;
	file->info.sectorbytes = sectorbytes;
	file->hunksectors = chd_get_header(chd)->hunkbytes / file->info.sectorbytes;
	file->cachehunk = -1;
	
	/* allocate a cache */
	file->cache = malloc(chd_get_header(chd)->hunkbytes);
	if (!file->cache)
	{
		free(file);
		return NULL;
	}
	
	return file;
}



/*************************************
 *
 *	Close a hard disk
 *
 *************************************/

void hard_disk_close(struct hard_disk_file *file)
{
	/* free the cache */
	if (file->cache)
		free(file->cache);
	free(file);
}



/*************************************
 *
 *	Return the handle to the CHD
 *
 *************************************/

struct chd_file *hard_disk_get_chd(struct hard_disk_file *file)
{
	return file->chd;
}



/*************************************
 *
 *	Return hard disk specific info
 *
 *************************************/

struct hard_disk_info *hard_disk_get_info(struct hard_disk_file *file)
{
	return &file->info;
}



/*************************************
 *
 *	Read from a hard disk
 *
 *************************************/

uint32_t hard_disk_read(struct hard_disk_file *file, uint32_t lbasector, uint32_t numsectors, void *buffer)
{
	uint32_t hunknum = lbasector / file->hunksectors;
	uint32_t sectoroffs = lbasector % file->hunksectors;

	/* for now, just break down multisector reads into single sectors */
	if (numsectors > 1)
	{
		uint32_t total = 0;
		while (numsectors--)
		{
			if (hard_disk_read(file, lbasector++, 1, (uint8_t *)buffer + total * file->info.sectorbytes))
				total++;
			else
				break;
		}
		return total;
	}

	/* if we haven't cached this hunk, read it now */
	if (file->cachehunk != hunknum)
	{
		if (!chd_read(file->chd, hunknum, 1, file->cache))
			return 0;
		file->cachehunk = hunknum;
	}
	
	/* copy out the requested sector */
	memcpy(buffer, &file->cache[sectoroffs * file->info.sectorbytes], file->info.sectorbytes);
	return 1;
}



/*************************************
 *
 *	Write to a hard disk
 *
 *************************************/

uint32_t hard_disk_write(struct hard_disk_file *file, uint32_t lbasector, uint32_t numsectors, const void *buffer)
{
	uint32_t hunknum = lbasector / file->hunksectors;
	uint32_t sectoroffs = lbasector % file->hunksectors;
	
	/* for now, just break down multisector writes into single sectors */
	if (numsectors > 1)
	{
		uint32_t total = 0;
		while (numsectors--)
		{
			if (hard_disk_write(file, lbasector++, 1, (const uint8_t *)buffer + total * file->info.sectorbytes))
				total++;
			else
				break;
		}
		return total;
	}

	/* if we haven't cached this hunk, read it now */
	if (file->cachehunk != hunknum)
	{
		if (!chd_read(file->chd, hunknum, 1, file->cache))
			return 0;
		file->cachehunk = hunknum;
	}
	
	/* copy in the requested data */
	memcpy(&file->cache[sectoroffs * file->info.sectorbytes], buffer, file->info.sectorbytes);
	
	/* write it back out */
	if (chd_write(file->chd, hunknum, 1, file->cache))
		return 1;
	return 0;
}
