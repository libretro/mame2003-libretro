/*##########################################################################

	atarimo.h

	Common motion object management functions for Atari raster games.

##########################################################################*/

#ifndef __ATARIMO__
#define __ATARIMO__


/*##########################################################################
	CONSTANTS
##########################################################################*/

/* maximum number of motion object processors */
#define ATARIMO_MAX				2

/* maximum objects per bank */
#define ATARIMO_MAXPERBANK		1024

/* shift to get to priority in raw data */
#define ATARIMO_PRIORITY_SHIFT	12
#define ATARIMO_PRIORITY_MASK	((~0 << ATARIMO_PRIORITY_SHIFT) & 0xffff)
#define ATARIMO_DATA_MASK		(ATARIMO_PRIORITY_MASK ^ 0xffff)



/*##########################################################################
	TYPES & STRUCTURES
##########################################################################*/

/* callback for special processing */
typedef int (*atarimo_special_cb)(struct mame_bitmap *bitmap, const struct rectangle *clip, int code, int color, int xpos, int ypos, struct rectangle *mobounds);

/* description for a four-word mask */
struct atarimo_entry
{
	uint16_t			data[4];
};

/* description of the motion objects */
struct atarimo_desc
{
	uint8_t				gfxindex;			/* index to which gfx system */
	uint8_t				banks;				/* number of motion object banks */
	uint8_t				linked;				/* are the entries linked? */
	uint8_t				split;				/* are the entries split? */
	uint8_t				reverse;			/* render in reverse order? */
	uint8_t				swapxy;				/* render in swapped X/Y order? */
	uint8_t				nextneighbor;		/* does the neighbor bit affect the next object? */
	uint16_t				slipheight;			/* pixels per SLIP entry (0 for no-slip) */
	uint8_t				slipoffset;			/* pixel offset for SLIPs */
	uint16_t				maxlinks;			/* maximum number of links to visit/scanline (0=all) */

	uint16_t				palettebase;		/* base palette entry */
	uint16_t				maxcolors;			/* maximum number of colors */
	uint8_t				transpen;			/* transparent pen index */

	struct atarimo_entry linkmask;			/* mask for the link */
	struct atarimo_entry gfxmask;			/* mask for the graphics bank */
	struct atarimo_entry codemask;			/* mask for the code index */
	struct atarimo_entry codehighmask;		/* mask for the upper code index */
	struct atarimo_entry colormask;			/* mask for the color */
	struct atarimo_entry xposmask;			/* mask for the X position */
	struct atarimo_entry yposmask;			/* mask for the Y position */
	struct atarimo_entry widthmask;			/* mask for the width, in tiles*/
	struct atarimo_entry heightmask;		/* mask for the height, in tiles */
	struct atarimo_entry hflipmask;			/* mask for the horizontal flip */
	struct atarimo_entry vflipmask;			/* mask for the vertical flip */
	struct atarimo_entry prioritymask;		/* mask for the priority */
	struct atarimo_entry neighbormask;		/* mask for the neighbor */
	struct atarimo_entry absolutemask;		/* mask for absolute coordinates */

	struct atarimo_entry specialmask;		/* mask for the special value */
	uint16_t			specialvalue;		/* resulting value to indicate "special" */
	atarimo_special_cb	specialcb;			/* callback routine for special entries */
};

/* rectangle list */
struct atarimo_rect_list
{
	int					numrects;
	struct rectangle *	rect;
};


/*##########################################################################
	FUNCTION PROTOTYPES
##########################################################################*/

/* setup/shutdown */
int atarimo_init(int map, const struct atarimo_desc *desc);
uint16_t *atarimo_get_code_lookup(int map, int *size);
uint8_t *atarimo_get_color_lookup(int map, int *size);
uint8_t *atarimo_get_gfx_lookup(int map, int *size);

/* core processing */
struct mame_bitmap *atarimo_render(int map, const struct rectangle *cliprect, struct atarimo_rect_list *rectlist);

/* atrribute setters */
void atarimo_set_bank(int map, int bank);
void atarimo_set_palettebase(int map, int base);
void atarimo_set_xscroll(int map, int xscroll);
void atarimo_set_yscroll(int map, int yscroll);

/* atrribute getters */
int atarimo_get_bank(int map);
int atarimo_get_palettebase(int map);
int atarimo_get_xscroll(int map);
int atarimo_get_yscroll(int map);

/* write handlers */
WRITE16_HANDLER( atarimo_0_spriteram_w );
WRITE16_HANDLER( atarimo_0_spriteram_expanded_w );
WRITE16_HANDLER( atarimo_0_slipram_w );

WRITE16_HANDLER( atarimo_1_spriteram_w );
WRITE16_HANDLER( atarimo_1_slipram_w );



/*##########################################################################
	GLOBAL VARIABLES
##########################################################################*/

extern uint16_t *atarimo_0_spriteram;
extern uint16_t *atarimo_0_slipram;

extern uint16_t *atarimo_1_spriteram;
extern uint16_t *atarimo_1_slipram;


#endif
