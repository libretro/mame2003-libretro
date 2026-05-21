void cdb_build_ftree(void);
void cdb_build_toc(void);
void iso_reset(void);
void CD_com_update(uint32_t count);
uint32_t cdb_find_track(uint32_t fad);
uint32_t cdb_find_file(uint32_t fad);
void cdb_inject_file_info(uint32_t fid, uint8_t * dst);


#define CDB_SEND_REPORT()								\
		if((CD_status == CDB_STAT_BUSY) ||					\
		   (CD_status == CDB_STAT_STDBY) ||					\
		   (CD_status == CDB_STAT_OPEN) ||					\
		   (CD_status == CDB_STAT_NODISC) ||					\
		   (CD_status == CDB_STAT_REJECT)){					\
			CR1 = (CD_status << 8) | 0xff;				\
			CR2 = 0xffff;							\
			CR3 = 0xffff;							\
			CR4 = 0xffff;							\
		}else{									\
			CR1 = (CD_status << 8) | CD_flag | CD_repeat;	\
			CR2 = (CD_cur_ctrl << 8) | CD_cur_track;		\
			CR3 = (CD_cur_idx  << 8) | (CD_cur_fad >> 16);	\
			CR4 = CD_cur_fad;						\
		}



#define CDB_FLAG_CDROM		0x80		// on CDROM area (else CDDA, <SEEK> or <SCAN>)

extern uint8_t 	CD_cr_first;
extern uint8_t 	CD_cr_writing;
extern uint16_t	CR1;
extern uint16_t	CR2;
extern uint16_t	CR3;
extern uint16_t	CR4;
extern uint16_t	CD_hirq;
extern uint16_t 	CD_hirq_i;
extern uint16_t  CD_mask;
extern uint8_t	CD_status;
extern uint32_t	CD_fad;
extern uint8_t	CD_track;
extern uint8_t	CD_control;
extern uint8_t	CD_index;

extern uint32_t fn;

extern int32_t	CD_com;				// last command being processed
extern int32_t	CD_com_play;			// last play command
extern uint8_t	CD_stat;				// drive status
extern uint8_t	CD_flag;				// 0x00 = CD-DA or <SEEK> or <SCAN> 0x80 = CD-ROM

#define CDB_SECT_NUM	200		// 200 sectors can be contained in the cd buffer
#define CDB_SEL_NUM	24		// 24 selectors (buffer partitions / filters) available
#define CDB_FID_NUM	1000		// number of total file ids (directory info) to keep track in the fid array

typedef struct filt_t {

	uint8_t		true_cond;		// true connection
	uint8_t		false_cond;		// false connection
	uint8_t		mode;		// filter mode
	uint32_t		fad;		// range start fad
	uint32_t		range;		// range length in fads
	uint8_t		chan;		// channel
	uint8_t		fid;		// file id
	uint8_t		sub_val;	// subheader value
	uint8_t		sub_mask;	// subheader mask
	uint8_t		cod_val;	// code value
	uint8_t		cod_mask;	// code mask

} filt_t;


typedef struct track_t {

	uint32_t		ctrl;		// track control
	uint32_t		idx;		// track index
	uint32_t		fad;		// track fad
	uint32_t		min;		// track time, minute
	uint32_t		sec;		// track time, second
	uint32_t		fra;		// track time, frame
	uint32_t		num;		// track number (just here for speed)

} track_t;


typedef struct sect_t {

	uint8_t		dummy1[2048];	// for MSCDEX
	uint8_t		data[2352];	// sector data
	uint8_t		dummy2[2048];	// for MSCDEX
	uint32_t		size;		// used space in bytes
	uint32_t		fad;		// sector fad
	uint8_t		fid;		// file id
	uint8_t		chan;		// channel
	uint8_t		sub;		// submode info
	uint8_t		cod;		// code info

} sect_t;

typedef struct part_t {

	uint32_t		size;				// partition size in bytes (also points to the last used sector)
	sect_t	* sect[CDB_SECT_NUM];	// sector info pointers

} part_t;


typedef struct toc_t {

	track_t	track[100];	// tracks (0 ~ 99)
	track_t	first;		// first track
	track_t	last;		// last track
	track_t	leadout;	// leadout (last session)

} toc_t;

typedef struct file_t {

	uint32_t		fad;		// file data fad
	uint32_t		size;		// file data size in bytes
	uint8_t		attr;		// file attributes
	uint8_t		unit;		// interleave unit
	uint8_t		gap;		// interleave gap
	uint8_t		name_len;	// file name length
	char		name[33];	// file name

} file_t;





extern uint32_t	CD_last_part;			// last buffer partition accessed
extern filt_t	CD_filt[CDB_SEL_NUM];		// filters

extern uint32_t	CD_play_fad;			// play start address
extern uint32_t	CD_play_range;			// play range
extern uint32_t	CD_seek_target;			// seek target address
extern uint8_t	CD_scan_dir;			// scan direction
extern uint32_t	CD_search_pn;			// search result, partition number
extern uint32_t	CD_search_sp;			// search result, sector position
extern uint32_t	CD_search_fad;			// search result, fad
extern uint32_t	CD_file_scope_first;
extern uint32_t	CD_file_scope_last;


extern uint32_t	CD_data_pn;			// data transfer partition number
extern uint32_t	CD_data_sp;			// data transfer sector position
extern uint32_t	CD_data_sn;			// data transfer sector number
extern uint32_t	CD_data_count;			// data transfer current byte count
extern uint32_t	CD_data_delete;			// data must be deleted upon read
extern uint32_t	CD_data_size;			// data transfer size in bytes

extern char * 	CD_info_ptr;			// info transfer buffer pointer
extern uint32_t	CD_info_count;			// info transfer byte count
extern uint32_t	CD_info_size;			// info transfer total byte count

extern uint32_t	CD_trans_type;			// 0 = DATA, 1 = INFO   //maybe signed int

extern uint32_t	CD_actual_size;			// used by "calcactualsize" and "getactualsize"

////////////////////////////////////////////////////////////////

extern sect_t	CD_sect[CDB_SECT_NUM];	// sector buffer
extern part_t	CD_part[CDB_SEL_NUM];	// buffer partitions
extern filt_t	CD_filt[CDB_SEL_NUM];	// filters
extern uint32_t	CD_free_space;		// free space in sector units

extern uint8_t	CD_filt_num;		// cdrom drive connector
extern uint8_t	CD_mpeg_filt_num;	// mpeg connector

////////////////////////////////////////////////////////////////

//************
#define CDB_STAT_BUSY		0x00		// status change in progress
#define CDB_STAT_PAUSE		0x01		// temporarily stopped
#define CDB_STAT_STDBY		0x02		// stopped
#define CDB_STAT_PLAY		0x03		// play in progress
#define CDB_STAT_SEEK		0x04		// seeking
#define CDB_STAT_SCAN		0x05		// scanning
#define CDB_STAT_OPEN		0x06		// tray open
#define CDB_STAT_NODISC		0x07		// no disc
#define CDB_STAT_RETRY		0x08		// read retry in progress
#define CDB_STAT_ERROR		0x09		// read data error
#define CDB_STAT_FATAL		0x0a		// fatal error
#define CDB_STAT_PERI		0x20		// it is periodical response
#define CDB_STAT_TRNS		0x40		// transfer request
#define CDB_STAT_WAIT		0x80		// waiting
#define CDB_STAT_REJECT		0xff		// command rejected
//**********

#define CD_FLAG_CDROM		0x80		// on CDROM area (else CDDA, <SEEK> or <SCAN>)

#define CDB_FILTMODE_FID	0x01		// file number matters
#define CDB_FILTMODE_CHAN	0x02		// channel number matters
#define CDB_FILTMODE_SUB	0x04		// submode matters
#define CDB_FILTMODE_COD	0x08		// code info matters
#define CDB_FILTMODE_RANGE	0x40		// range matters

//************
#define HIRQ_CMOK			0x0001	// ready for command
#define HIRQ_DRDY			0x0002	// data transfer setup complete
#define HIRQ_CSCT			0x0004	// 1 sector stored
#define HIRQ_BFUL			0x0008	// buffer full
#define HIRQ_PEND			0x0010	// play ended
#define HIRQ_DCHG			0x0020	// disk changed
#define HIRQ_ESEL			0x0040	// soft reset/selector finished
#define HIRQ_EHST			0x0080	// host i/o finished
#define HIRQ_ECPY			0x0100	// sector copy/move finished
#define HIRQ_EFLS			0x0200	// block file system finished
#define HIRQ_SCDQ			0x0400	// subcode q decoded for current sector
#define HIRQ_MPED			0x0800	// mpeg enabled ?
#define HIRQ_MPCM			0x1000	// mpeg command ?
#define HIRQ_MPST			0x2000	// mpeg status ?
//***********


extern uint32_t	CD_cur_fad;			// current pickup position info
extern uint32_t	CD_cur_track;			//
extern uint32_t	CD_cur_ctrl;			//
extern uint32_t	CD_cur_idx;			//
extern uint32_t	CD_cur_fid;			//



extern char	cdb_sat_file_info[254 * 12];	// current file info
extern char cdb_sat_subq[5 * 2];		// current subcode q
extern char cdb_sat_subrw[12 * 2];		// current subcode r~w
extern char CD_sat_subq[5 * 2];			// current subcode q
extern char CD_sat_subrw[12 * 2];		// current subcode r~w
extern toc_t	CD_toc;			// disc toc
extern file_t	CD_file[CDB_FID_NUM];	// file table (directory table)
extern uint32_t	CD_file_num;		// total file infos stored
extern char	CD_sat_toc[408];	// current cdrom toc


extern uint8_t	CD_init_flag;
extern uint8_t	CD_flag;			// 0x00 = CD-DA or <SEEK> or <SCAN> 0x80 = CD-ROM
extern uint32_t	CD_repeat;			// repeat frequency
extern uint32_t	CD_standby;			// standby wait
extern uint32_t	CD_repeat_max;			// max repeat frequency
extern uint8_t	CD_ecc;
extern uint32_t	CD_drive_speed;			// 0 = noop, 1 = 1x, 2 = 2x


extern uint8_t			cdda_buff[8192];		// CD-DA buffer for SCSP communication
extern uint32_t		cdda_pos;



