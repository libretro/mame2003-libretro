#ifndef __UNZIP_H
#define __UNZIP_H

#include "osd_cpu.h"
#include "mame2003.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
 * Support for retrieving files from zipfiles
 ***************************************************************************/

struct zipent {
	uint32_t	cent_file_header_sig;
	uint8_t	version_made_by;
	uint8_t	host_os;
	uint8_t	version_needed_to_extract;
	uint8_t	os_needed_to_extract;
	uint16_t	general_purpose_bit_flag;
	uint16_t	compression_method;
	uint16_t	last_mod_file_time;
	uint16_t	last_mod_file_date;
	uint32_t	crc32;
	uint32_t	compressed_size;
	uint32_t	uncompressed_size;
	uint16_t	filename_length;
	uint16_t	extra_field_length;
	uint16_t	file_comment_length;
	uint16_t	disk_number_start;
	uint16_t	internal_file_attrib;
	uint32_t	external_file_attrib;
	uint32_t	offset_lcl_hdr_frm_frst_disk;
	char*   name; /* 0 terminated */
};

typedef struct _ZIP {
	char* zip; /* zip name */
	FILE* fp; /* zip handler */
	int pathtype,pathindex;	/* additional path info */
	long length; /* length of zip file */

	char* ecd; /* end_of_cent_dir data */
	unsigned ecd_length; /* end_of_cent_dir length */

	char* cd; /* cent_dir data */

	unsigned cd_pos; /* position in cent_dir */

	struct zipent ent; /* buffer for readzip */

	/* end_of_cent_dir */
	uint32_t	end_of_cent_dir_sig;
	uint16_t	number_of_this_disk;
	uint16_t	number_of_disk_start_cent_dir;
	uint16_t	total_entries_cent_dir_this_disk;
	uint16_t	total_entries_cent_dir;
	uint32_t	size_of_cent_dir;
	uint32_t	offset_to_start_of_cent_dir;
	uint16_t	zipfile_comment_length;
	char*	zipfile_comment; /* pointer in ecd */
} ZIP;

/* Opens a zip stream for reading
   return:
     !=0 success, zip stream
     ==0 error
*/
ZIP* openzip(int pathtype, int pathindex, const char* path);

/* Closes a zip stream */
void closezip(ZIP* zip);

/* Reads the current entry from a zip stream
   in:
     zip opened zip
   return:
     !=0 success
     ==0 error
*/
struct zipent* readzip(ZIP* zip);

/* Suspend access to a zip file (release file handler)
   in:
      zip opened zip
   note:
     A suspended zip is automatically reopened at first call of
     readuncompressd() or readcompressed() functions
*/
void suspendzip(ZIP* zip);

/* Resets a zip stream to the first entry
   in:
     zip opened zip
   note:
     ZIP file must be opened and not suspended
*/
void rewindzip(ZIP* zip);

/* Read compressed data from a zip entry
   in:
     zip opened zip
     ent entry to read
   out:
     data buffer for data, ent.compressed_size UINT8s allocated by the caller
   return:
     ==0 success
     <0 error
*/
int readcompresszip(ZIP* zip, struct zipent* ent, char* data);

/* Read decompressed data from a zip entry
   in:
     zip zip stream open
     ent entry to read
   out:
     data buffer for data, ent.uncompressed_size UINT8s allocated by the caller
   return:
     ==0 success
     <0 error
*/
int readuncompresszip(ZIP* zip, struct zipent* ent, char* data);

/* public functions */
int /* error */ load_zipped_file (int pathtype, int pathindex, const char *zipfile, const char *filename,
	unsigned char **buf, unsigned int *length);
int /* error */ checksum_zipped_file (int pathtype, int pathindex, const char *zipfile, const char *filename, unsigned int *length, unsigned int *sum);

void unzip_cache_clear(void);

/* public globals */
extern int	gUnzipQuiet;	/* flag controls error messages */

#ifdef __cplusplus
}
#endif

#endif
