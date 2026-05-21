#ifndef _WINDOW_H_
#define	_WINDOW_H_

#include "osd_cpu.h"
#include "mamedbg.h"

#ifdef  GNU
#define ARGFMT  __attribute__((format(printf,2,3)))
#else
#define ARGFMT
#endif

#ifndef DECL_SPEC
#define DECL_SPEC
#endif

#ifndef TRUE
#define TRUE    1
#endif

#ifndef FALSE
#define FALSE   0
#endif

#ifndef INVALID
#define INVALID 0xffffffff
#endif

#ifndef WIN_EMPTY
#define WIN_EMPTY   176 /* checkered pattern */
#endif
#ifndef CAPTION_L
#define CAPTION_L   174 /* >> */
#endif
#ifndef CAPTION_R
#define CAPTION_R   175 /* << */
#endif
#ifndef FRAME_TL
#define FRAME_TL    218 /* top, left border */
#endif
#ifndef FRAME_BL
#define FRAME_BL    192 /* bottom, left border */
#endif
#ifndef FRAME_TR
#define FRAME_TR    191 /* top, right border */
#endif
#ifndef FRAME_BR
#define FRAME_BR    217 /* bottom, right border */
#endif
#ifndef FRAME_V
#define FRAME_V     179 /* vertical line */
#endif
#ifndef FRAME_H
#define FRAME_H     196 /* horizontal line */
#endif

/* This is our window structure */

struct sWindow
{
	uint8_t filler;		/* Character */
	uint8_t prio; 		/* This window's priority */
	uint32_t x;			/* X Position (in characters) of our window */
	uint32_t y;			/* Y Position (in characters) of our window */
	uint32_t w;			/* X Size of our window (in characters) */
	uint32_t h;			/* Y Size (lines) of our window (in character lengths) */
	uint32_t cx;			/* Current cursor's X position */
	uint32_t cy;			/* Current cursor's Y position */
	uint32_t flags;		/* Window's attributes (below) */
	uint8_t co_text;		/* Default color */
	uint8_t co_frame; 	/* Frame color */
	uint8_t co_title; 	/* Title color */
	uint8_t saved_text;	/* Character under the cursor position */
	uint8_t saved_attr;	/* Attribute under the cursor position */

	/* Stuff that needs to be saved off differently */

	char	*title; /* Window title (if any) */
	uint8_t	*text;	/* Pointer to video data - characters */
	uint8_t	*attr;	/* Pointer to video data - attributes */

	/* These are the callbacks when certain things happen. All fields have been
	 * updated BEFORE the call. Return FALSE if the moves, resizes, closes,
	 * refocus aren't accepted.
	 */

	uint32_t (*Resize)(uint32_t idx, struct sWindow *);
	uint32_t (*Close)(uint32_t idx, struct sWindow *);
	uint32_t (*Move)(uint32_t idx, struct sWindow *);
	uint32_t (*Refocus)(uint32_t idx, struct sWindow *);  /* Bring it to the front */
};

/* These defines are for various aspects of the window */

#define BORDER_LEFT 		0x01	/* Border on left side of window */
#define BORDER_RIGHT		0x02	/* Border on right side of window */
#define BORDER_TOP			0x04	/* Border on top side of window */
#define BORDER_BOTTOM		0x08	/* Border on bottom side of window */
#define HIDDEN				0x10	/* Is it hidden currently? */
#define CURSOR_ON			0x20	/* Is the cursor on? */
#define NO_WRAP 			0x40	/* Do we actually wrap at the right side? */
#define NO_SCROLL			0x80	/* Do we actually scroll it? */
#define SHADOW				0x100	/* Do we cast a shadow? */
#define MOVEABLE			0x200	/* Is this Window moveable? */
#define RESIZEABLE			0x400	/* IS this Window resiable? */

#define MAX_WINDOWS 		32		/* Up to 32 windows active at once */
#define TAB_STOP			8		/* 8 Spaces for a tab stop! */

#define AUTO_FIX_XYWH		TRUE
#define NEWLINE_ERASE_EOL	TRUE	/* Shall newline also erase to end of line? */

/* Special characters */

#define CHAR_CURSORON		219 /* Cursor on character */
#define CHAR_CURSOROFF		32	/* Cursor off character */

/* Standard color set for IBM character set. DO NOT ALTER! */

#define WIN_BLACK			DBG_BLACK
#define WIN_BLUE			DBG_BLUE
#define WIN_GREEN			DBG_GREEN
#define WIN_CYAN			DBG_CYAN
#define WIN_RED 			DBG_RED
#define WIN_MAGENTA 		DBG_MAGENTA
#define WIN_BROWN			DBG_BROWN
#define WIN_WHITE			DBG_LIGHTGRAY
#define WIN_GRAY			DBG_GRAY
#define WIN_LIGHT_BLUE		DBG_LIGHTBLUE
#define WIN_LIGHT_GREEN 	DBG_LIGHTGREEN
#define WIN_LIGHT_CYAN		DBG_LIGHTCYAN
#define WIN_LIGHT_RED		DBG_LIGHTRED
#define WIN_LIGHT_MAGENTA	DBG_LIGHTMAGENTA
#define WIN_YELLOW			DBG_YELLOW
#define WIN_BRIGHT_WHITE	DBG_WHITE

#define	WIN_BRIGHT	0x08

/* Externs! */

extern uint32_t screen_w;
extern uint32_t screen_h;

extern void win_erase_eol(uint32_t idx, uint8_t bChar);
extern int32_t win_putc(uint32_t idx, uint8_t bChar);
extern uint32_t win_open(uint32_t idx, struct sWindow *psWin);
extern uint32_t win_init_engine(uint32_t w, uint32_t h);
extern uint32_t win_is_initalized(uint32_t idx);
extern void win_exit_engine(void);
extern void win_close(uint32_t idx);
extern int32_t win_vprintf(uint32_t idx, const char *pszString, va_list arg);
extern int32_t DECL_SPEC win_printf(uint32_t idx, const char *pszString, ...) ARGFMT;
extern uint32_t DECL_SPEC win_set_title(uint32_t idx, const char *pszTitle, ... ) ARGFMT;
extern uint32_t win_get_cx(uint32_t idx);
extern uint32_t win_get_cy(uint32_t idx);
extern uint32_t win_get_cx_abs(uint32_t idx);
extern uint32_t win_get_cy_abs(uint32_t idx);
extern uint32_t win_get_x_abs(uint32_t idx);
extern uint32_t win_get_y_abs(uint32_t idx);
extern uint32_t win_get_w(uint32_t idx);
extern uint32_t win_get_h(uint32_t idx);
extern void win_set_w(uint32_t idx, uint32_t w);
extern void win_set_h(uint32_t idx, uint32_t w);
extern void win_set_color(uint32_t idx, uint32_t color);
extern void win_set_title_color(uint32_t idx, uint32_t color);
extern void win_set_frame_color(uint32_t idx, uint32_t color);
extern void win_set_curpos(uint32_t idx, uint32_t x, uint32_t y);
extern void win_set_cursor(uint32_t idx, uint32_t dwCursorState);
extern void win_hide(uint32_t idx);
extern void win_show(uint32_t idx);
extern void win_update(uint32_t idx);
extern uint8_t win_get_prio(uint32_t idx);
extern void win_set_prio(uint32_t idx, uint8_t prio);
extern void win_move(uint32_t idx, uint32_t dwX, uint32_t dwY);
extern void win_invalidate_video(void);

#endif
