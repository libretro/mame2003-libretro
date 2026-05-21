/***************************************************************************

	MAME Compressed Hunks of Data file format

***************************************************************************/

#ifndef CHD_H
#define CHD_H

#include "osd_cpu.h"


/***************************************************************************

	Compressed Hunks of Data header format. All numbers are stored in
	Motorola (big-endian) byte ordering. The header is 76 (V1) or 80 (V2)
	bytes long.

	V1 header:

	[  0] char   tag[8];		// 'MComprHD'
	[  8] uint32_t length;		// length of header (including tag and length fields)
	[ 12] uint32_t version;		// drive format version
	[ 16] uint32_t flags;			// flags (see below)
	[ 20] uint32_t compression;	// compression type
	[ 24] uint32_t hunksize;		// 512-byte sectors per hunk
	[ 28] uint32_t totalhunks;	// total # of hunks represented
	[ 32] uint32_t cylinders;		// number of cylinders on hard disk
	[ 36] uint32_t heads;			// number of heads on hard disk
	[ 40] uint32_t sectors;		// number of sectors on hard disk
	[ 44] uint8_t  md5[16];		// MD5 checksum of raw data
	[ 60] uint8_t  parentmd5[16];	// MD5 checksum of parent file
	[ 76] (V1 header length)

	V2 header:

	[  0] char   tag[8];		// 'MComprHD'
	[  8] uint32_t length;		// length of header (including tag and length fields)
	[ 12] uint32_t version;		// drive format version
	[ 16] uint32_t flags;			// flags (see below)
	[ 20] uint32_t compression;	// compression type
	[ 24] uint32_t hunksize;		// seclen-byte sectors per hunk
	[ 28] uint32_t totalhunks;	// total # of hunks represented
	[ 32] uint32_t cylinders;		// number of cylinders on hard disk
	[ 36] uint32_t heads;			// number of heads on hard disk
	[ 40] uint32_t sectors;		// number of sectors on hard disk
	[ 44] uint8_t  md5[16];		// MD5 checksum of raw data
	[ 60] uint8_t  parentmd5[16];	// MD5 checksum of parent file
	[ 76] uint32_t seclen;		// number of bytes per sector
	[ 80] (V2 header length)

	V3 header:

	[  0] char   tag[8];		// 'MComprHD'
	[  8] uint32_t length;		// length of header (including tag and length fields)
	[ 12] uint32_t version;		// drive format version
	[ 16] uint32_t flags;			// flags (see below)
	[ 20] uint32_t compression;	// compression type
	[ 24] uint32_t totalhunks;	// total # of hunks represented
	[ 28] uint64_t logicalbytes;	// logical size of the data (in bytes)
	[ 36] uint64_t metaoffset;	// offset to the first blob of metadata
	[ 44] uint8_t  md5[16];		// MD5 checksum of raw data
	[ 60] uint8_t  parentmd5[16];	// MD5 checksum of parent file
	[ 76] uint32_t hunkbytes;		// number of bytes per hunk
	[ 80] uint8_t  sha1[20];		// SHA1 checksum of raw data
	[100] uint8_t  parentsha1[20];// SHA1 checksum of parent file
	[120] (V3 header length)

	Flags:
		0x00000001 - set if this drive has a parent
		0x00000002 - set if this drive allows writes

***************************************************************************/

/*************************************
 *
 *	Constants
 *
 *************************************/

#define CHD_HEADER_VERSION			3
#define CHD_V1_HEADER_SIZE			76
#define CHD_V2_HEADER_SIZE			80
#define CHD_V3_HEADER_SIZE			120
#define CHD_MAX_HEADER_SIZE			CHD_V3_HEADER_SIZE

#define CHD_MD5_BYTES				16
#define CHD_SHA1_BYTES				20

#define CHDFLAGS_HAS_PARENT			0x00000001
#define CHDFLAGS_IS_WRITEABLE		0x00000002
#define CHDFLAGS_UNDEFINED			0xfffffffc

#define CHDCOMPRESSION_NONE			0
#define CHDCOMPRESSION_ZLIB			1
#define CHDCOMPRESSION_ZLIB_PLUS	2
#define CHDCOMPRESSION_MAX			3

#define CHD_MAX_METADATA_SIZE		4096
#define CHDMETATAG_WILDCARD			0
#define CHD_METAINDEX_APPEND		((uint32_t)-1)

#define HARD_DISK_STANDARD_METADATA	0x47444444
#define HARD_DISK_METADATA_FORMAT	"CYLS:%d,HEADS:%d,SECS:%d,BPS:%d"

enum
{
	CHDERR_NONE = 0,
	CHDERR_NO_INTERFACE,
	CHDERR_OUT_OF_MEMORY,
	CHDERR_INVALID_FILE,
	CHDERR_INVALID_PARAMETER,
	CHDERR_INVALID_DATA,
	CHDERR_FILE_NOT_FOUND,
	CHDERR_REQUIRES_PARENT,
	CHDERR_FILE_NOT_WRITEABLE,
	CHDERR_READ_ERROR,
	CHDERR_WRITE_ERROR,
	CHDERR_CODEC_ERROR,
	CHDERR_INVALID_PARENT,
	CHDERR_HUNK_OUT_OF_RANGE,
	CHDERR_DECOMPRESSION_ERROR,
	CHDERR_COMPRESSION_ERROR,
	CHDERR_CANT_CREATE_FILE,
	CHDERR_CANT_VERIFY,
	CHDERR_NOT_SUPPORTED,
	CHDERR_METADATA_NOT_FOUND,
	CHDERR_INVALID_METADATA_SIZE,
	CHDERR_UNSUPPORTED_VERSION
};




/*************************************
 *
 *	Type definitions
 *
 *************************************/

struct chd_header
{
	uint32_t	length;						/* length of header data */
	uint32_t	version;					/* drive format version */
	uint32_t	flags;						/* flags field */
	uint32_t	compression;				/* compression type */
	uint32_t	hunkbytes;					/* number of bytes per hunk */
	uint32_t	totalhunks;					/* total # of hunks represented */
	uint64_t	logicalbytes;				/* logical size of the data */
	uint64_t	metaoffset;					/* offset in file of first metadata */
	uint8_t	md5[CHD_MD5_BYTES];			/* MD5 checksum of raw data */
	uint8_t	parentmd5[CHD_MD5_BYTES];	/* MD5 checksum of parent file */
	uint8_t	sha1[CHD_SHA1_BYTES];		/* SHA1 checksum of raw data */
	uint8_t	parentsha1[CHD_SHA1_BYTES];	/* SHA1 checksum of parent file */

	uint32_t	obsolete_cylinders;			/* obsolete field -- do not use! */
	uint32_t	obsolete_sectors;			/* obsolete field -- do not use! */
	uint32_t	obsolete_heads;				/* obsolete field -- do not use! */
	uint32_t	obsolete_hunksize;			/* obsolete field -- do not use! */
};


struct chd_file;
struct chd_interface_file;

struct chd_interface
{
	struct chd_interface_file *(*open)(const char *filename, const char *mode);
	void (*close)(struct chd_interface_file *file);
	uint32_t (*read)(struct chd_interface_file *file, uint64_t offset, uint32_t count, void *buffer);
	uint32_t (*write)(struct chd_interface_file *file, uint64_t offset, uint32_t count, const void *buffer);
	uint64_t (*length)(struct chd_interface_file *file);
};



/*************************************
 *
 *	Prototypes
 *
 *************************************/

void chd_set_interface(struct chd_interface *interface);
void chd_save_interface(struct chd_interface *interface_save);

int chd_create(const char *filename, uint64_t logicalbytes, uint32_t hunkbytes, uint32_t compression, struct chd_file *parent);
struct chd_file *chd_open(const char *filename, int writeable, struct chd_file *parent);
void chd_close(struct chd_file *chd);
void chd_close_all(void);

uint32_t chd_get_metadata(struct chd_file *chd, uint32_t *metatag, uint32_t metaindex, void *outputbuf, uint32_t outputlen);
int chd_set_metadata(struct chd_file *chd, uint32_t metatag, uint32_t metaindex, const void *inputbuf, uint32_t inputlen);

uint32_t chd_read(struct chd_file *chd, uint32_t hunknum, uint32_t hunkcount, void *buffer);
uint32_t chd_write(struct chd_file *chd, uint32_t hunknum, uint32_t hunkcount, const void *buffer);

int chd_get_last_error(void);
const struct chd_header *chd_get_header(struct chd_file *chd);
int chd_set_header(const char *filename, const struct chd_header *header);

int chd_compress(struct chd_file *chd, const char *rawfile, uint32_t offset, void (*progress)(const char *, ...));
int chd_verify(struct chd_file *chd, void (*progress)(const char *, ...), uint8_t actualmd5[CHD_MD5_BYTES], uint8_t actualsha1[CHD_SHA1_BYTES]);

#endif /* CHD_H */
