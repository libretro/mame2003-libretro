
#ifndef MAME_PNG_H
#define MAME_PNG_H


#define PNG_Signature       "\x89\x50\x4E\x47\x0D\x0A\x1A\x0A"
#define MNG_Signature       "\x8A\x4D\x4E\x47\x0D\x0A\x1A\x0A"

#define PNG_CN_IHDR 0x49484452L     /* Chunk names */
#define PNG_CN_PLTE 0x504C5445L
#define PNG_CN_IDAT 0x49444154L
#define PNG_CN_IEND 0x49454E44L
#define PNG_CN_gAMA 0x67414D41L
#define PNG_CN_sBIT 0x73424954L
#define PNG_CN_cHRM 0x6348524DL
#define PNG_CN_tRNS 0x74524E53L
#define PNG_CN_bKGD 0x624B4744L
#define PNG_CN_hIST 0x68495354L
#define PNG_CN_tEXt 0x74455874L
#define PNG_CN_zTXt 0x7A545874L
#define PNG_CN_pHYs 0x70485973L
#define PNG_CN_oFFs 0x6F464673L
#define PNG_CN_tIME 0x74494D45L
#define PNG_CN_sCAL 0x7343414CL

#define PNG_PF_None     0   /* Prediction filters */
#define PNG_PF_Sub      1
#define PNG_PF_Up       2
#define PNG_PF_Average  3
#define PNG_PF_Paeth    4

#define MNG_CN_MHDR 0x4D484452L     /* MNG Chunk names */
#define MNG_CN_MEND 0x4D454E44L
#define MNG_CN_TERM 0x5445524DL
#define MNG_CN_BACK 0x4241434BL


/* PNG support */
struct png_info {
	uint32_t width, height;
	uint32_t xres, yres;
	struct rectangle screen;
	double xscale, yscale;
	double source_gamma;
	uint32_t chromaticities[8];
	uint32_t resolution_unit, offset_unit, scale_unit;
	uint8_t bit_depth;
	uint32_t significant_bits[4];
	uint32_t background_color[4];
	uint8_t color_type;
	uint8_t compression_method;
	uint8_t filter_method;
	uint8_t interlace_method;
	uint32_t num_palette;
	uint8_t *palette;
	uint32_t num_trans;
	uint8_t *trans;
	uint8_t *image;

	/* The rest is private and should not be used
	 * by the public functions
	 */
	uint8_t bpp;
	uint32_t rowbytes;
	uint8_t *zimage;
	uint32_t zlength;
	uint8_t *fimage;
};

int png_verify_signature (mame_file *fp);
int png_inflate_image (struct png_info *p);
int png_read_file(mame_file *fp, struct png_info *p);
#if 0
int png_read_info(mame_file *fp, struct png_info *p);
#endif
int png_expand_buffer_8bit (struct png_info *p);
void png_delete_unused_colors (struct png_info *p);
int png_add_text (const char *keyword, const char *text);
int png_unfilter(struct png_info *p);
int png_filter(struct png_info *p);
int png_deflate_image(struct png_info *p);
int png_write_sig(mame_file *fp);
int png_write_datastream(mame_file *fp, struct png_info *p);
int png_write_bitmap(mame_file *fp, struct mame_bitmap *bitmap);
int mng_capture_start(mame_file *fp, struct mame_bitmap *bitmap);
int mng_capture_frame(mame_file *fp, struct mame_bitmap *bitmap);
int mng_capture_stop(mame_file *fp);
int mng_capture_status(void);
#endif
