/***************************************************************************

	PSX GPU

	Preliminary software renderer by smf.
	Thanks to Ryan Holtz, Pete B & Farfetch'd.

	Supports:
	  type 1 1024x1024 framebuffer (CXD8538Q)
	  type 2 1024x512 framebuffer
	  type 2 1024x1024 framebuffer (CXD8514Q/CXD8561Q/CXD8654Q)

	Debug Keys:
		M toggles mesh viewer.
		V toggles vram viewer.
		I toggles interleave in vram viewer.

***************************************************************************/

#include "driver.h"
#include "state.h"
#include "includes/psx.h"
#include "usrintrf.h"

#define STOP_ON_ERROR ( 0 )

#define VERBOSE_LEVEL ( 0 )

static INLINE void verboselog( int n_level, const char *s_fmt, ... )
{
	if( VERBOSE_LEVEL >= n_level )
	{
		va_list v;
		char buf[ 32768 ];
		va_start( v, s_fmt );
		vsprintf( buf, s_fmt, v );
		va_end( v );
		logerror( "%08x: %s", activecpu_get_pc(), buf );
	}
}

struct FLATVERTEX
{
	PAIR n_coord;
};

struct GOURAUDVERTEX
{
	PAIR n_bgr;
	PAIR n_coord;
};

struct FLATTEXTUREDVERTEX
{
	PAIR n_coord;
	PAIR n_texture;
};

struct GOURAUDTEXTUREDVERTEX
{
	PAIR n_bgr;
	PAIR n_coord;
	PAIR n_texture;
};

static union
{
	uint32_t n_entry[ 16 ];

	struct
	{
		PAIR n_cmd;
		struct FLATVERTEX vertex[ 2 ];
		PAIR n_size;
	} MoveImage;

	struct
	{
		PAIR n_bgr;
		PAIR n_coord;
		PAIR n_size;
	} FlatRectangle;

	struct
	{
		PAIR n_bgr;
		PAIR n_coord;
		PAIR n_texture;
	} Sprite8x8;

	struct
	{
		PAIR n_bgr;
		PAIR n_coord;
		PAIR n_texture;
	} Sprite16x16;

	struct
	{
		PAIR n_bgr;
		PAIR n_coord;
		PAIR n_texture;
		PAIR n_size;
	} FlatTexturedRectangle;

	struct
	{
		PAIR n_bgr;
		struct FLATVERTEX vertex[ 4 ];
	} FlatPolygon;

	struct
	{
		struct GOURAUDVERTEX vertex[ 4 ];
	} GouraudPolygon;

	struct
	{
		PAIR n_bgr;
		struct FLATVERTEX vertex[ 2 ];
	} MonochromeLine;

	struct
	{
		struct GOURAUDVERTEX vertex[ 2 ];
	} GouraudLine;

	struct
	{
		PAIR n_bgr;
		struct FLATTEXTUREDVERTEX vertex[ 4 ];
	} FlatTexturedPolygon;

	struct
	{
		struct GOURAUDTEXTUREDVERTEX vertex[ 4 ];
	} GouraudTexturedPolygon;

	struct
	{
		PAIR n_bgr;
		struct FLATVERTEX vertex;
	} Dot;
} m_packet;

static uint16_t *m_p_vram;
static uint32_t m_n_vram_size;
static uint32_t m_n_gpu_buffer_offset;
static uint32_t m_n_vramx;
static uint32_t m_n_vramy;
static uint32_t m_n_twy;
static uint32_t m_n_twx;
static uint32_t m_n_twh;
static uint32_t m_n_tww;
static uint32_t m_n_drawarea_x1;
static uint32_t m_n_drawarea_y1;
static uint32_t m_n_drawarea_x2;
static uint32_t m_n_drawarea_y2;
static uint32_t m_n_horiz_disstart;
static uint32_t m_n_horiz_disend;
static uint32_t m_n_vert_disstart;
static uint32_t m_n_vert_disend;
static uint32_t m_b_reverseflag;
static int32_t m_n_drawoffset_x;
static int32_t m_n_drawoffset_y;
static uint32_t m_n_displaystartx;
static uint32_t m_n_displaystarty;
static uint32_t m_n_gpustatus;
static uint32_t m_n_gpuinfo;
static uint32_t m_n_screenwidth;
static uint32_t m_n_screenheight;
static uint32_t m_n_drawmode;

#define MAX_LEVEL ( 32 )
#define MID_LEVEL ( ( MAX_LEVEL / 2 ) << 8 )
#define MAX_SHADE ( 0x100 )
#define MID_SHADE ( 0x80 )

static int m_n_gputype;

static uint16_t *m_p_p_vram[ 1024 ];

static uint16_t m_p_n_redshade[ MAX_LEVEL * MAX_SHADE ];
static uint16_t m_p_n_greenshade[ MAX_LEVEL * MAX_SHADE ];
static uint16_t m_p_n_blueshade[ MAX_LEVEL * MAX_SHADE ];
static uint16_t m_p_n_redlevel[ 0x10000 ];
static uint16_t m_p_n_greenlevel[ 0x10000 ];
static uint16_t m_p_n_bluelevel[ 0x10000 ];

static uint16_t m_p_n_f025[ MAX_LEVEL * MAX_SHADE ];
static uint16_t m_p_n_f05[ MAX_LEVEL * MAX_SHADE ];
static uint16_t m_p_n_f1[ MAX_LEVEL * MAX_SHADE ];
static uint16_t m_p_n_redb05[ 0x10000 ];
static uint16_t m_p_n_greenb05[ 0x10000 ];
static uint16_t m_p_n_blueb05[ 0x10000 ];
static uint16_t m_p_n_redb1[ 0x10000 ];
static uint16_t m_p_n_greenb1[ 0x10000 ];
static uint16_t m_p_n_blueb1[ 0x10000 ];
static uint16_t m_p_n_redaddtrans[ MAX_LEVEL * MAX_LEVEL ];
static uint16_t m_p_n_greenaddtrans[ MAX_LEVEL * MAX_LEVEL ];
static uint16_t m_p_n_blueaddtrans[ MAX_LEVEL * MAX_LEVEL ];
static uint16_t m_p_n_redsubtrans[ MAX_LEVEL * MAX_LEVEL ];
static uint16_t m_p_n_greensubtrans[ MAX_LEVEL * MAX_LEVEL ];
static uint16_t m_p_n_bluesubtrans[ MAX_LEVEL * MAX_LEVEL ];
static uint16_t m_p_n_nextpointlist4[] = { 1, 3, 0, 2 };
static uint16_t m_p_n_prevpointlist4[] = { 2, 0, 3, 1 };
static uint16_t m_p_n_nextpointlist3[] = { 1, 2, 0 };
static uint16_t m_p_n_prevpointlist3[] = { 2, 0, 1 };
static uint16_t m_p_n_nextpointlist4b[] = { 0, 3, 1, 2 };
static uint16_t m_p_n_prevpointlist4b[] = { 0, 2, 3, 1 };

#define SINT11( x ) ( ( (int32_t)( x ) << 21 ) >> 21 )

#define ADJUST_COORD( a ) \
	a.w.l = COORD_X( a ) + m_n_drawoffset_x; \
	a.w.h = COORD_Y( a ) + m_n_drawoffset_y;

#define COORD_X( a ) ( (int16_t)a.w.l )
#define COORD_Y( a ) ( (int16_t)a.w.h )

/* The GPU rejects any primitive whose vertices span more than 1023 pixels in
   x or y; without this oversized garbage polygons get rasterised. A macro, so
   it is always expanded regardless of optimisation level (a and b are pure
   coordinate reads, so the double evaluation is harmless). */
#define CullVertex( a, b ) ( ( (a) - (b) ) < -1023 || ( (a) - (b) ) > 1023 )

#define CULLPOINT( PacketType, p1, p2 ) \
( \
	CullVertex( COORD_Y( m_packet.PacketType.vertex[ p1 ].n_coord ), COORD_Y( m_packet.PacketType.vertex[ p2 ].n_coord ) ) || \
	CullVertex( COORD_X( m_packet.PacketType.vertex[ p1 ].n_coord ), COORD_X( m_packet.PacketType.vertex[ p2 ].n_coord ) ) \
)

#define CULLTRIANGLE( PacketType, start ) \
( \
	CULLPOINT( PacketType, start, start + 1 ) || CULLPOINT( PacketType, start + 1, start + 2 ) || CULLPOINT( PacketType, start + 2, start ) \
)

#define FINDTOPLEFT( PacketType ) \
	for( n_point = 0; n_point < n_points; n_point++ ) \
	{ \
		ADJUST_COORD( m_packet.PacketType.vertex[ n_point ].n_coord ); \
	} \
	\
	n_leftpoint = 0; \
	if( n_points == 4 ) \
	{ \
		if( CULLTRIANGLE( PacketType, 0 ) ) \
		{ \
			if( CULLTRIANGLE( PacketType, 1 ) ) \
			{ \
				return; \
			} \
			p_n_rightpointlist = m_p_n_nextpointlist4b; \
			p_n_leftpointlist = m_p_n_prevpointlist4b; \
			n_leftpoint++; \
		} \
		else if( CULLTRIANGLE( PacketType, 1 ) ) \
		{ \
			p_n_rightpointlist = m_p_n_nextpointlist3; \
			p_n_leftpointlist = m_p_n_prevpointlist3; \
			n_points--; \
		} \
		else \
		{ \
			p_n_rightpointlist = m_p_n_nextpointlist4; \
			p_n_leftpointlist = m_p_n_prevpointlist4; \
		} \
	} \
	else if( CULLTRIANGLE( PacketType, 0 ) ) \
	{ \
		return; \
	} \
	else \
	{ \
		p_n_rightpointlist = m_p_n_nextpointlist3; \
		p_n_leftpointlist = m_p_n_prevpointlist3; \
	} \
	\
	for( n_point = n_leftpoint + 1; n_point < n_points; n_point++ ) \
	{ \
		if( COORD_Y( m_packet.PacketType.vertex[ n_point ].n_coord ) < COORD_Y( m_packet.PacketType.vertex[ n_leftpoint ].n_coord ) || \
			( COORD_Y( m_packet.PacketType.vertex[ n_point ].n_coord ) == COORD_Y( m_packet.PacketType.vertex[ n_leftpoint ].n_coord ) && \
			COORD_X( m_packet.PacketType.vertex[ n_point ].n_coord ) < COORD_X( m_packet.PacketType.vertex[ n_leftpoint ].n_coord ) ) ) \
		{ \
			n_leftpoint = n_point; \
		} \
	} \
	n_rightpoint = n_leftpoint;
#define SIZE_W( a ) ( a.w.l )
#define SIZE_H( a ) ( a.w.h )
#define BGR_C( a ) ( a.b.h3 )
#define BGR_B( a ) ( a.b.h2 )
#define BGR_G( a ) ( a.b.h )
#define BGR_R( a ) ( a.b.l )
#define TEXTURE_V( a ) ( (uint8_t)a.b.h )
#define TEXTURE_U( a ) ( (uint8_t)a.b.l )

PALETTE_INIT( psx )
{
	uint32_t n_r;
	uint32_t n_g;
	uint32_t n_b;
	uint32_t n_colour;

	for( n_colour = 0; n_colour < 0x10000; n_colour++ )
	{
		n_r = ( ( n_colour & 0x1f ) * 0xff ) / 0x1f;
		n_g = ( ( ( n_colour >> 5 ) & 0x1f ) * 0xff ) / 0x1f;
		n_b = ( ( ( n_colour >> 10 ) & 0x1f ) * 0xff ) / 0x1f;

		palette_set_color( n_colour, n_r, n_g, n_b );
	}
}

#if defined( MAME_DEBUG )

#define DEBUG_COORDS ( 10 )
static struct mame_bitmap *debugmesh;
static int m_b_debugclear;
static int m_b_debugmesh;
static int m_n_debugskip;
static int m_b_debugtexture;
static int m_n_debuginterleave;
static int m_n_debugcoord;
static int m_n_debugcoordx[ DEBUG_COORDS ];
static int m_n_debugcoordy[ DEBUG_COORDS ];

#define DEBUG_MAX ( 512 )

static void DebugMeshInit( void )
{
	m_b_debugmesh = 0;
	m_b_debugtexture = 0;
	m_n_debuginterleave = -1;
	m_b_debugclear = 1;
	m_n_debugcoord = 0;
	m_n_debugskip = 0;
	debugmesh = auto_bitmap_alloc_depth( 1024, 1024, 16 );
}

static void DebugMesh( int n_coordx, int n_coordy )
{
	int n_coord;
	int n_colour;

	if( m_b_debugclear )
	{
		fillbitmap( debugmesh, 0x0000, NULL );
		m_b_debugclear = 0;
	}

	if( m_n_debugcoord < DEBUG_COORDS )
	{
		n_coordx += m_n_displaystartx;
		n_coordy += m_n_displaystarty;

		n_coordx *= 511;
		n_coordx /= DEBUG_MAX - 1;
		n_coordx += 256;
		n_coordy *= 511;
		n_coordy /= DEBUG_MAX - 1;
		n_coordy += 256;

		m_n_debugcoordx[ m_n_debugcoord ] = n_coordx;
		m_n_debugcoordy[ m_n_debugcoord ] = n_coordy;
		m_n_debugcoord++;
	}

	n_colour = 0x1f;
	for( n_coord = 0; n_coord < m_n_debugcoord; n_coord++ )
	{
		if( n_coordx != m_n_debugcoordx[ n_coord ] ||
			n_coordy != m_n_debugcoordy[ n_coord ] )
		{
			break;
		}
	}
	if( n_coord == m_n_debugcoord && m_n_debugcoord > 1 )
	{
		n_colour = 0xffff;
	}
	for( n_coord = 0; n_coord < m_n_debugcoord; n_coord++ )
	{
		PAIR n_x;
		PAIR n_y;
		int32_t n_xstart;
		int32_t n_ystart;
		int32_t n_xend;
		int32_t n_yend;
		int32_t n_xlen;
		int32_t n_ylen;
		int32_t n_len;
		int32_t n_dx;
		int32_t n_dy;

		n_xstart = m_n_debugcoordx[ n_coord ];
		n_xend = n_coordx;
		if( n_xend > n_xstart )
		{
			n_xlen = n_xend - n_xstart;
		}
		else
		{
			n_xlen = n_xstart - n_xend;
		}

		n_ystart = m_n_debugcoordy[ n_coord ];
		n_yend = n_coordy;
		if( n_yend > n_ystart )
		{
			n_ylen = n_yend - n_ystart;
		}
		else
		{
			n_ylen = n_ystart - n_yend;
		}

		if( n_xlen > n_ylen )
		{
			n_len = n_xlen;
		}
		else
		{
			n_len = n_ylen;
		}

		n_x.w.h = n_xstart; n_x.w.l = 0;
		n_y.w.h = n_ystart; n_y.w.l = 0;

		if( n_len == 0 )
		{
			n_len = 1;
		}

		n_dx = (int32_t)( ( n_xend << 16 ) - n_x.d ) / n_len;
		n_dy = (int32_t)( ( n_yend << 16 ) - n_y.d ) / n_len;
		while( n_len > 0 )
		{
			if( (int16_t)n_x.w.h >= 0 &&
				(int16_t)n_y.w.h >= 0 &&
				(int16_t)n_x.w.h <= 1023 &&
				(int16_t)n_y.w.h <= 1023 )
			{
				if( read_pixel( debugmesh, n_x.w.h, n_y.w.h ) != 0xffff )
				{
					plot_pixel( debugmesh, n_x.w.h, n_y.w.h, n_colour );
				}
			}
			n_x.d += n_dx;
			n_y.d += n_dy;
			n_len--;
		}
	}
}

static void DebugMeshEnd( void )
{
	m_n_debugcoord = 0;
}

static int DebugMeshDisplay( struct mame_bitmap *bitmap, const struct rectangle *cliprect )
{
	if( keyboard_pressed_memory( KEYCODE_M ) )
	{
		m_b_debugmesh = !m_b_debugmesh;
	}
	if( m_b_debugmesh )
	{
		set_visible_area( 0, 1023, 0, 1023 );
		copybitmap( bitmap, debugmesh, 0, 0, 0, 0, cliprect, TRANSPARENCY_NONE, 0 );
	}
	m_b_debugclear = 1;
	return m_b_debugmesh;
}

static int DebugTextureDisplay( struct mame_bitmap *bitmap )
{
	uint32_t n_y;

	if( keyboard_pressed_memory( KEYCODE_V ) )
	{
		m_b_debugtexture = !m_b_debugtexture;
	}
	if( m_b_debugtexture )
	{
		if( keyboard_pressed_memory( KEYCODE_I ) )
		{
			m_n_debuginterleave++;
			if( m_n_debuginterleave == 2 )
			{
				m_n_debuginterleave = -1;
			}
			if( m_n_debuginterleave == -1 )
			{
				usrintf_showmessage_secs( 1, "interleave off" );
			}
			else if( m_n_debuginterleave == 0 )
			{
				usrintf_showmessage_secs( 1, "4 bit interleave" );
			}
			else if( m_n_debuginterleave == 1 )
			{
				usrintf_showmessage_secs( 1, "8 bit interleave" );
			}
		}
		set_visible_area( 0, 1023, 0, 1023 );

		for( n_y = 0; n_y < 1024; n_y++ )
		{
			int n_x;
			int n_xi;
			int n_yi;
			unsigned short p_n_interleave[ 1024 ];

			for( n_x = 0; n_x < 1024; n_x++ )
			{
				if( m_n_debuginterleave == 0 )
				{
					n_xi = ( n_x & ~0x3c ) + ( ( n_y << 2 ) & 0x3c );
					n_yi = ( n_y & ~0xf ) + ( ( n_x >> 2 ) & 0xf );
				}
				else if( m_n_debuginterleave == 1 )
				{
					n_xi = ( n_x & ~0x78 ) + ( ( n_x << 3 ) & 0x40 ) + ( ( n_y << 3 ) & 0x38 ); \
					n_yi = ( n_y & ~0x7 ) + ( ( n_x >> 4 ) & 0x7 ); \
				}
				else
				{
					n_xi = n_x;
					n_yi = n_y;
				}
				p_n_interleave[ n_x ] = m_p_p_vram[ n_yi ][ n_xi ];
			}

         const uint16_t *src = &p_n_interleave;
         int dy = bitmap->rowpixels;
         uint16_t *dst = (uint16_t *)bitmap->base + y * dy;
         int length = 1023;

         while (length--)
            *dst++ = *src++;
		}
	}
	return m_b_debugtexture;
}

#endif

static int psx_gpu_init( int n_width, int n_height )
{
	int n_line;
	int n_level;
	int n_level2;
	int n_shade;
	int n_shaded;

#if defined( MAME_DEBUG )
	DebugMeshInit();
#endif

	m_n_gpustatus = 0x14802000;
	m_n_drawmode = 0;
	m_n_gpuinfo = 0;
	m_n_gpu_buffer_offset = 0;

	m_n_vram_size = n_width * n_height;
	m_p_vram = auto_malloc( m_n_vram_size * 2 );
	if( m_p_vram == NULL )
	{
		return 1;
	}
	memset( m_p_vram, 0x00, m_n_vram_size * 2 );

	for( n_line = 0; n_line < 1024; n_line++ )
	{
		m_p_p_vram[ n_line ] = &m_p_vram[ ( n_line % n_height ) * n_width ];
	}

	for( n_level = 0; n_level < MAX_LEVEL; n_level++ )
	{
		for( n_shade = 0; n_shade < MAX_SHADE; n_shade++ )
		{
			/* shaded */
			n_shaded = ( n_level * n_shade ) / MID_SHADE;
			if( n_shaded > MAX_LEVEL - 1 )
			{
				n_shaded = MAX_LEVEL - 1;
			}
			m_p_n_redshade[ ( n_level * MAX_SHADE ) | n_shade ] = n_shaded;
			m_p_n_greenshade[ ( n_level * MAX_SHADE ) | n_shade ] = n_shaded << 5;
			m_p_n_blueshade[ ( n_level * MAX_SHADE ) | n_shade ] = n_shaded << 10;

			/* 1/4 x transparency */
			n_shaded = ( n_level * n_shade ) / MID_SHADE;
			n_shaded >>= 2;
			if( n_shaded > MAX_LEVEL - 1 )
			{
				n_shaded = MAX_LEVEL - 1;
			}
			m_p_n_f025[ ( n_level * MAX_SHADE ) | n_shade ] = n_shaded;

			/* 1/2 x transparency */
			n_shaded = ( n_level * n_shade ) / MID_SHADE;
			n_shaded >>= 1;
			if( n_shaded > MAX_LEVEL - 1 )
			{
				n_shaded = MAX_LEVEL - 1;
			}
			m_p_n_f05[ ( n_level * MAX_SHADE ) | n_shade ] = n_shaded;

			/* 1 x transparency */
			n_shaded = ( n_level * n_shade ) / MID_SHADE;
			if( n_shaded > MAX_LEVEL - 1 )
			{
				n_shaded = MAX_LEVEL - 1;
			}
			m_p_n_f1[ ( n_level * MAX_SHADE ) | n_shade ] = n_shaded;
		}
	}

	for( n_level = 0; n_level < 0x10000; n_level++ )
	{
		m_p_n_redlevel[ n_level ] = ( n_level & ( MAX_LEVEL - 1 ) ) * MAX_SHADE;
		m_p_n_greenlevel[ n_level ] = ( ( n_level >> 5 ) & ( MAX_LEVEL - 1 ) ) * MAX_SHADE;
		m_p_n_bluelevel[ n_level ] = ( ( n_level >> 10 ) & ( MAX_LEVEL - 1 ) ) * MAX_SHADE;

		/* 0.5 * background */
		m_p_n_redb05[ n_level ] = ( ( n_level & ( MAX_LEVEL - 1 ) ) / 2 ) * MAX_LEVEL;
		m_p_n_greenb05[ n_level ] = ( ( ( n_level >> 5 ) & ( MAX_LEVEL - 1 ) ) / 2 ) * MAX_LEVEL;
		m_p_n_blueb05[ n_level ] = ( ( ( n_level >> 10 ) & ( MAX_LEVEL - 1 ) ) / 2 ) * MAX_LEVEL;

		/* 1 * background */
		m_p_n_redb1[ n_level ] = ( n_level & ( MAX_LEVEL - 1 ) ) * MAX_LEVEL;
		m_p_n_greenb1[ n_level ] = ( ( n_level >> 5 ) & ( MAX_LEVEL - 1 ) ) * MAX_LEVEL;
		m_p_n_blueb1[ n_level ] = ( ( n_level >> 10 ) & ( MAX_LEVEL - 1 ) ) * MAX_LEVEL;

	}

	for( n_level = 0; n_level < MAX_LEVEL; n_level++ )
	{
		for( n_level2 = 0; n_level2 < MAX_LEVEL; n_level2++ )
		{
			/* add transparency */
			n_shaded = ( n_level + n_level2 );
			if( n_shaded > MAX_LEVEL - 1 )
			{
				n_shaded = MAX_LEVEL - 1;
			}
			m_p_n_redaddtrans[ ( n_level * MAX_LEVEL ) | n_level2 ] = n_shaded;
			m_p_n_greenaddtrans[ ( n_level * MAX_LEVEL ) | n_level2 ] = n_shaded << 5;
			m_p_n_blueaddtrans[ ( n_level * MAX_LEVEL ) | n_level2 ] = n_shaded << 10;

			/* sub transparency */
			n_shaded = ( n_level - n_level2 );
			if( n_shaded < 0 )
			{
				n_shaded = 0;
			}
			m_p_n_redsubtrans[ ( n_level * MAX_LEVEL ) | n_level2 ] = n_shaded;
			m_p_n_greensubtrans[ ( n_level * MAX_LEVEL ) | n_level2 ] = n_shaded << 5;
			m_p_n_bluesubtrans[ ( n_level * MAX_LEVEL ) | n_level2 ] = n_shaded << 10;
		}
	}

	state_save_register_UINT8( "psx", 0, "m_packet", (uint8_t *)&m_packet, sizeof( m_packet ) );
	state_save_register_UINT16( "psx", 0, "m_p_vram", m_p_vram, m_n_vram_size );
	state_save_register_UINT32( "psx", 0, "m_n_gpu_buffer_offset", &m_n_gpu_buffer_offset, 1 );
	state_save_register_UINT32( "psx", 0, "m_n_vramx", &m_n_vramx, 1 );
	state_save_register_UINT32( "psx", 0, "m_n_vramy", &m_n_vramy, 1 );
	state_save_register_UINT32( "psx", 0, "m_n_twy", &m_n_twy, 1 );
	state_save_register_UINT32( "psx", 0, "m_n_twx", &m_n_twx, 1 );
	state_save_register_UINT32( "psx", 0, "m_n_twh", &m_n_tww, 1 );
	state_save_register_UINT32( "psx", 0, "m_n_drawarea_x1", &m_n_drawarea_x1, 1 );
	state_save_register_UINT32( "psx", 0, "m_n_drawarea_y1", &m_n_drawarea_y1, 1 );
	state_save_register_UINT32( "psx", 0, "m_n_drawarea_x2", &m_n_drawarea_x2, 1 );
	state_save_register_UINT32( "psx", 0, "m_n_drawarea_y2", &m_n_drawarea_y2, 1 );
	state_save_register_UINT32( "psx", 0, "m_n_horiz_disstart", &m_n_horiz_disstart, 1 );
	state_save_register_UINT32( "psx", 0, "m_n_horiz_disend", &m_n_horiz_disend, 1 );
	state_save_register_UINT32( "psx", 0, "m_n_vert_disstart", &m_n_vert_disstart, 1 );
	state_save_register_UINT32( "psx", 0, "m_n_vert_disend", &m_n_vert_disend, 1 );
	state_save_register_UINT32( "psx", 0, "m_b_reverseflag", &m_b_reverseflag, 1 );
	state_save_register_INT32( "psx", 0, "m_n_drawoffset_x", &m_n_drawoffset_x, 1 );
	state_save_register_INT32( "psx", 0, "m_n_drawoffset_y", &m_n_drawoffset_y, 1 );
	state_save_register_UINT32( "psx", 0, "m_n_displaystartx", &m_n_displaystartx, 1 );
	state_save_register_UINT32( "psx", 0, "m_n_displaystarty", &m_n_displaystarty, 1 );
	state_save_register_UINT32( "psx", 0, "m_n_gpustatus", &m_n_gpustatus, 1 );
	state_save_register_UINT32( "psx", 0, "m_n_gpuinfo", &m_n_gpuinfo, 1 );
	state_save_register_UINT32( "psx", 0, "m_n_screenwidth", &m_n_screenwidth, 1 );
	state_save_register_UINT32( "psx", 0, "m_n_screenheight", &m_n_screenheight, 1 );
	state_save_register_UINT32( "psx", 0, "m_n_drawmode", &m_n_drawmode, 1 );

	return 0;
}

VIDEO_START( psx_type1_1024x1024 )
{
	m_n_gputype = 1;
	return psx_gpu_init( 1024, 1024 );
}

VIDEO_START( psx_type2_1024x512 )
{
	m_n_gputype = 2;
	return psx_gpu_init( 1024, 512 );
}

VIDEO_START( psx_type2_1024x1024 )
{
	m_n_gputype = 2;
	return psx_gpu_init( 1024, 1024 );
}

VIDEO_STOP( psx )
{
}

/* Set to 0 by a driver that composites on top of the game bitmap after the
   GPU readout (e.g. a light-gun game drawing crosshairs), so the readout uses
   the bitmap path and the overlay survives. 1 = direct framebuffer allowed. */
static int psx_direct_fb_ok = 1;

void psx_enable_direct_fb( int enable )
{
	psx_direct_fb_ok = enable;
}

VIDEO_UPDATE( psx )
{
	uint32_t n_x;
	uint32_t n_y;

#if defined( MAME_DEBUG )
	if( DebugMeshDisplay( bitmap, cliprect ) )
	{
		return;
	}
	if( DebugTextureDisplay( bitmap ) )
	{
		return;
	}
#if 0
	if( keyboard_pressed_memory( KEYCODE_I ) )
	{
		m_n_debugskip++;
		if( m_n_debugskip > 13 )
		{
			m_n_debugskip = 0;
		}
		usrintf_showmessage_secs( 1, "debug skip %d", m_n_debugskip );
	}
	if( keyboard_pressed_memory( KEYCODE_D ) )
	{
		FILE *f;
		int n_x;
		f = fopen( "dump.txt", "w" );
		for( n_y = 256; n_y < 512; n_y++ )
		{
			for( n_x = 640; n_x < 1024; n_x++ )
			{
				fprintf( f, "%04u,%04u = %04x\n", n_y, n_x, m_p_p_vram[ n_y ][ n_x ] );
			}
		}
		fclose( f );
	}
	if( keyboard_pressed_memory( KEYCODE_S ) )
	{
		FILE *f;
		usrintf_showmessage_secs( 1, "saving..." );
		f = fopen( "VRAM.BIN", "wb" );
		for( n_y = 0; n_y < 1024; n_y++ )
		{
			fwrite( m_p_p_vram[ n_y ], 1024 * 2, 1, f );
		}
		fclose( f );
	}
	if( keyboard_pressed_memory( KEYCODE_L ) )
	{
		FILE *f;
		usrintf_showmessage_secs( 1, "loading..." );
		f = fopen( "VRAM.BIN", "rb" );
		for( n_y = 0; n_y < 1024; n_y++ )
		{
			fread( m_p_p_vram[ n_y ], 1024 * 2, 1, f );
		}
		fclose( f );
	}
#endif
#endif

	set_visible_area( 0, m_n_screenwidth - 1, 0, m_n_screenheight - 1 );

	if( ( m_n_gpustatus & ( 1 << 0x17 ) ) != 0 )
	{
		/* todo: only draw to necessary area */
      int sy, ey, sx, ex, y;
      sy = cliprect->min_y;
      ey = cliprect->max_y;
      sx = cliprect->min_x;
      ex = cliprect->max_x;

      for (y = sy;y <= ey;y++)
         memset(((uint16_t *)bitmap->line[y]) + sx, 0, (ex-sx+1)*2);
	}
	else
	{
		/* todo: clear border */
		if( m_b_reverseflag )
		{
			n_x = ( 1023 - m_n_displaystartx );
			/* todo: make this flip the screen, in the meantime.. */
			n_x -= ( m_n_screenwidth - 1 );
		}
		else
		{
			n_x = m_n_displaystartx;
		}

		/* Fast path: paint RGB565 straight into the frontend framebuffer,
		   skipping the game bitmap and the conversion pass. The bitmap path
		   stores the raw 15bpp VRAM value as the pen; the palette is a fixed
		   RGB555->RGB565 decode that never changes, so the fast path engages
		   every frame. Falls back to the bitmap path when the output is not
		   RGB565, a flip/rotate/artwork/UI overlay is in the path, or a driver
		   has disabled it to composite on top of the bitmap. */
		{
			unsigned palents;
			unsigned fb_pitch;
			const uint16_t *pal = psx_direct_fb_ok ? mame2003_direct_rgb565_palette( &palents ) : NULL;
			uint16_t *fb = pal ? (uint16_t *)mame2003_direct_rgb565_begin( &fb_pitch ) : NULL;
			(void)palents;

			if( fb )
			{
				for( n_y = 0; n_y < m_n_screenheight; n_y++ )
				{
					const uint16_t *src = m_p_p_vram[ n_y + m_n_displaystarty ] + n_x;
					uint16_t *dst = (uint16_t *)( (uint8_t *)fb + n_y * fb_pitch );
					int length = m_n_screenwidth;

					while( length-- )
						*dst++ = pal[ *src++ ];
				}
				return;
			}
		}

		for( n_y = 0; n_y < m_n_screenheight; n_y++ )
      {
         const uint16_t *src = (m_p_p_vram[ n_y + m_n_displaystarty ] + n_x);
         int dy = bitmap->rowpixels;
         uint16_t *dst = (uint16_t *)bitmap->base + n_y * dy;
         int length = m_n_screenwidth;

         while (length--)
            *dst++ = *src++;
      }
	}
}

/*
type 1
f  e| d| c  b| a  9| 8  7| 6  5| 4| 3  2  1  0
    |ti|     |   tp|  abr|   ty|  |         tx
*/

/*
type 2
f  e  d  c| b| a  9| 8  7| 6  5| 4| 3  2  1  0
          |ty|     |   tp|  abr|ty|         tx
*/

#define TRANSPARENCYSETUP \
	p_n_f = m_p_n_f1; \
	p_n_redb = m_p_n_redb1; \
	p_n_greenb = m_p_n_greenb1; \
	p_n_blueb = m_p_n_blueb1; \
	p_n_redtrans = m_p_n_redaddtrans; \
	p_n_greentrans = m_p_n_greenaddtrans; \
	p_n_bluetrans = m_p_n_blueaddtrans; \
 \
	switch( n_cmd & 0x02 ) \
	{ \
	case 0x02: \
		switch( n_abr & 0x03 ) \
		{ \
		case 0x00: \
			p_n_f = m_p_n_f05; \
			p_n_redb = m_p_n_redb05; \
			p_n_greenb = m_p_n_greenb05; \
			p_n_blueb = m_p_n_blueb05; \
			p_n_redtrans = m_p_n_redaddtrans; \
			p_n_greentrans = m_p_n_greenaddtrans; \
			p_n_bluetrans = m_p_n_blueaddtrans; \
			verboselog( 2, "Transparency Mode: 0.5*B + 0.5*F\n" ); \
			break; \
		case 0x01: \
			p_n_f = m_p_n_f1; \
			p_n_redb = m_p_n_redb1; \
			p_n_greenb = m_p_n_greenb1; \
			p_n_blueb = m_p_n_blueb1; \
			p_n_redtrans = m_p_n_redaddtrans; \
			p_n_greentrans = m_p_n_greenaddtrans; \
			p_n_bluetrans = m_p_n_blueaddtrans; \
			verboselog( 2, "Transparency Mode: 1.0*B + 1.0*F\n" ); \
			break; \
		case 0x02: \
			p_n_f = m_p_n_f1; \
			p_n_redb = m_p_n_redb1; \
			p_n_greenb = m_p_n_greenb1; \
			p_n_blueb = m_p_n_blueb1; \
			p_n_redtrans = m_p_n_redsubtrans; \
			p_n_greentrans = m_p_n_greensubtrans; \
			p_n_bluetrans = m_p_n_bluesubtrans; \
			verboselog( 2, "Transparency Mode: 1.0*B - 1.0*F\n" ); \
			break; \
		case 0x03: \
			p_n_f = m_p_n_f025; \
			p_n_redb = m_p_n_redb1; \
			p_n_greenb = m_p_n_greenb1; \
			p_n_blueb = m_p_n_blueb1; \
			p_n_redtrans = m_p_n_redaddtrans; \
			p_n_greentrans = m_p_n_greenaddtrans; \
			p_n_bluetrans = m_p_n_blueaddtrans; \
			verboselog( 2, "Transparency Mode: 1.0*B + 0.25*F\n" ); \
			break; \
		} \
		break; \
	}

#define SOLIDSETUP( DRAWMODE ) \
	if( m_n_gputype == 2 ) \
	{ \
		n_abr = ( DRAWMODE & 0x60 ) >> 5; \
	} \
	else \
	{ \
		n_abr = ( DRAWMODE & 0x180 ) >> 7; \
	} \
	TRANSPARENCYSETUP

#define TEXTURESETUP( DRAWMODE ) \
	p_clut = m_p_p_vram[ n_cluty ] + n_clutx; \
	if( m_n_gputype == 2 ) \
	{ \
		if( m_n_drawmode != DRAWMODE ) \
		{ \
			m_n_drawmode = DRAWMODE; \
			m_n_gpustatus = ( m_n_gpustatus & 0xfffff800 ) | ( m_n_drawmode & 0x7ff ); \
		} \
		n_tx = ( DRAWMODE & 0x0f ) << 6; \
		n_ty = ( ( DRAWMODE & 0x10 ) << 4 ) | \
			( ( DRAWMODE & 0x800 ) >> 2 ); \
		n_abr = ( DRAWMODE & 0x60 ) >> 5; \
		n_tp = ( DRAWMODE & 0x180 ) >> 7; \
		n_ti = 0; \
	} \
	else \
	{ \
		if( m_n_drawmode != DRAWMODE ) \
		{ \
			m_n_drawmode = DRAWMODE; \
			m_n_gpustatus = ( m_n_gpustatus & 0xffffe000 ) | ( m_n_drawmode & 0x1fff ); \
		} \
		n_tx = ( DRAWMODE & 0x0f ) << 6; \
		n_ty = ( ( DRAWMODE & 0x60 ) << 3 ); \
		n_abr = ( DRAWMODE & 0x180 ) >> 7; \
		n_tp = ( DRAWMODE & 0x600 ) >> 9; \
		n_ti = ( DRAWMODE & 0x2000 ) >> 13; \
	} \
 \
	switch( n_tp ) \
	{ \
	case 0: \
		n_tx += m_n_twx >> 2; \
		n_ty += m_n_twy >> 2; \
		break; \
	case 1: \
		n_tx += m_n_twx >> 1; \
		n_ty += m_n_twy >> 1; \
		break; \
	case 2: \
		n_tx += m_n_twx >> 0; \
		n_ty += m_n_twy >> 0; \
		break; \
	} \
	TRANSPARENCYSETUP

#define FLATPOLYGONUPDATE
#define FLATRECTANGEUPDATE
#define GOURAUDPOLYGONUPDATE \
	n_r.d += n_dr; \
	n_g.d += n_dg; \
	n_b.d += n_db;

#define SOLIDFILL( PIXELUPDATE ) \
	if( n_distance > ( (int32_t)m_n_drawarea_x2 - n_x ) + 1 ) \
	{ \
		n_distance = ( m_n_drawarea_x2 - n_x ) + 1; \
	} \
	p_vram = m_p_p_vram[ n_y ] + n_x; \
 \
	switch( n_cmd & 0x02 ) \
	{ \
	case 0x00: \
		/* transparency off */ \
		while( n_distance > 0 ) \
		{ \
			*( p_vram ) = m_p_n_redshade[ MID_LEVEL | n_r.w.h ] | \
				m_p_n_greenshade[ MID_LEVEL | n_g.w.h ] | \
				m_p_n_blueshade[ MID_LEVEL | n_b.w.h ]; \
			p_vram++; \
			PIXELUPDATE \
			n_distance--; \
		} \
		break; \
	case 0x02: \
		/* transparency on */ \
		while( n_distance > 0 ) \
		{ \
			*( p_vram ) = p_n_redtrans[ p_n_f[ MID_LEVEL | n_r.w.h ] | p_n_redb[ *( p_vram ) ] ] | \
				p_n_greentrans[ p_n_f[ MID_LEVEL | n_g.w.h ] | p_n_greenb[ *( p_vram ) ] ] | \
				p_n_bluetrans[ p_n_f[ MID_LEVEL | n_b.w.h ] | p_n_blueb[ *( p_vram ) ] ]; \
			p_vram++; \
			PIXELUPDATE \
			n_distance--; \
		} \
		break; \
	} \

#define FLATTEXTUREDPOLYGONUPDATE \
	n_u.d += n_du; \
	n_v.d += n_dv;

#define GOURAUDTEXTUREDPOLYGONUPDATE \
	n_r.d += n_dr; \
	n_g.d += n_dg; \
	n_b.d += n_db; \
	n_u.d += n_du; \
	n_v.d += n_dv;

#define FLATTEXTUREDRECTANGLEUPDATE \
	n_u++;

#define TEXTURE4BIT( TXV, TXU ) \
	while( n_distance > 0 ) \
	{ \
		n_bgr = p_clut[ ( *( m_p_p_vram[ n_ty + TXV ] + n_tx + ( TXU >> 2 ) ) >> ( ( TXU & 0x03 ) << 2 ) ) & 0x0f ];

#define TEXTURE8BIT( TXV, TXU ) \
	while( n_distance > 0 ) \
	{ \
		n_bgr = p_clut[ ( *( m_p_p_vram[ n_ty + TXV ] + n_tx + ( TXU >> 1 ) ) >> ( ( TXU & 0x01 ) << 3 ) ) & 0xff ];

#define TEXTURE15BIT( TXV, TXU ) \
	while( n_distance > 0 ) \
	{ \
		n_bgr = *( m_p_p_vram[ n_ty + TXV ] + n_tx + TXU );

#define TEXTUREWINDOW4BIT( TXV, TXU ) TEXTURE4BIT( ( TXV % m_n_twh ), ( TXU % m_n_tww ) )
#define TEXTUREWINDOW8BIT( TXV, TXU ) TEXTURE8BIT( ( TXV % m_n_twh ), ( TXU % m_n_tww ) )
#define TEXTUREWINDOW15BIT( TXV, TXU ) TEXTURE15BIT( ( TXV % m_n_twh ), ( TXU % m_n_tww ) )

#define TEXTUREINTERLEAVED4BIT( TXV, TXU ) \
	while( n_distance > 0 ) \
	{ \
		int n_xi = ( ( TXU >> 2 ) & ~0x3c ) + ( ( TXV << 2 ) & 0x3c ); \
		int n_yi = ( TXV & ~0xf ) + ( ( TXU >> 4 ) & 0xf ); \
		n_bgr = p_clut[ ( *( m_p_p_vram[ n_ty + n_yi ] + n_tx + n_xi ) >> ( ( TXU & 0x03 ) << 2 ) ) & 0x0f ];

#define TEXTUREINTERLEAVED8BIT( TXV, TXU ) \
	while( n_distance > 0 ) \
	{ \
		int n_xi = ( ( TXU >> 1 ) & ~0x78 ) + ( ( TXU << 2 ) & 0x40 ) + ( ( TXV << 3 ) & 0x38 ); \
		int n_yi = ( TXV & ~0x7 ) + ( ( TXU >> 5 ) & 0x7 ); \
		n_bgr = p_clut[ ( *( m_p_p_vram[ n_ty + n_yi ] + n_tx + n_xi ) >> ( ( TXU & 0x01 ) << 3 ) ) & 0xff ];

#define TEXTUREINTERLEAVED15BIT( TXV, TXU ) \
	while( n_distance > 0 ) \
	{ \
		int n_xi = TXU; \
		int n_yi = TXV; \
		n_bgr = *( m_p_p_vram[ n_ty + n_yi ] + n_tx + n_xi );

#define TEXTUREWINDOWINTERLEAVED4BIT( TXV, TXU ) TEXTUREINTERLEAVED4BIT( ( TXV % m_n_twh ), ( TXU % m_n_tww ) )
#define TEXTUREWINDOWINTERLEAVED8BIT( TXV, TXU ) TEXTUREINTERLEAVED8BIT( ( TXV % m_n_twh ), ( TXU % m_n_tww ) )
#define TEXTUREWINDOWINTERLEAVED15BIT( TXV, TXU ) TEXTUREINTERLEAVED15BIT( ( TXV % m_n_twh ), ( TXU % m_n_tww ) )

#define SHADEDPIXEL( PIXELUPDATE ) \
		if( n_bgr != 0 ) \
		{ \
			*( p_vram ) = m_p_n_redshade[ m_p_n_redlevel[ n_bgr ] | n_r.w.h ] | \
				m_p_n_greenshade[ m_p_n_greenlevel[ n_bgr ] | n_g.w.h ] | \
				m_p_n_blueshade[ m_p_n_bluelevel[ n_bgr ] | n_b.w.h ]; \
		} \
		p_vram++; \
		PIXELUPDATE \
		n_distance--; \
	}

#define TRANSPARENTPIXEL( PIXELUPDATE ) \
		if( n_bgr != 0 ) \
		{ \
			if( ( n_bgr & 0x8000 ) != 0 ) \
			{ \
				*( p_vram ) = p_n_redtrans[ p_n_f[ m_p_n_redlevel[ n_bgr ] | n_r.w.h ] | p_n_redb[ *( p_vram ) ] ] | \
					p_n_greentrans[ p_n_f[ m_p_n_greenlevel[ n_bgr ] | n_g.w.h ] | p_n_greenb[ *( p_vram ) ] ] | \
					p_n_bluetrans[ p_n_f[ m_p_n_bluelevel[ n_bgr ] | n_b.w.h ] | p_n_blueb[ *( p_vram ) ] ]; \
			} \
			else \
			{ \
				*( p_vram ) = m_p_n_redshade[ m_p_n_redlevel[ n_bgr ] | n_r.w.h ] | \
					m_p_n_greenshade[ m_p_n_greenlevel[ n_bgr ] | n_g.w.h ] | \
					m_p_n_blueshade[ m_p_n_bluelevel[ n_bgr ] | n_b.w.h ]; \
			} \
		} \
		p_vram++; \
		PIXELUPDATE \
		n_distance--; \
	}

#define TEXTUREFILL( PIXELUPDATE, TXU, TXV ) \
	if( n_distance > ( (int32_t)m_n_drawarea_x2 - n_x ) + 1 ) \
	{ \
		n_distance = ( m_n_drawarea_x2 - n_x ) + 1; \
	} \
	p_vram = m_p_p_vram[ n_y ] + n_x; \
 \
	if( n_ti != 0 ) \
	{ \
		/* interleaved texture */ \
		if( m_n_twh != 256 || \
			m_n_tww != 256 || \
			m_n_twx != 0 || \
			m_n_twy != 0 ) \
		{ \
			/* texture window */ \
			switch( n_cmd & 0x02 ) \
			{ \
			case 0x00: \
				/* shading */ \
				switch( n_tp ) \
				{ \
				case 0: \
					/* 4 bit clut */ \
					TEXTUREWINDOWINTERLEAVED4BIT( TXV, TXU ) \
					SHADEDPIXEL( PIXELUPDATE ) \
					break; \
				case 1: \
					/* 8 bit clut */ \
					TEXTUREWINDOWINTERLEAVED8BIT( TXV, TXU ) \
					SHADEDPIXEL( PIXELUPDATE ) \
					break; \
				case 2: \
					/* 15 bit */ \
					TEXTUREWINDOWINTERLEAVED15BIT( TXV, TXU ) \
					SHADEDPIXEL( PIXELUPDATE ) \
					break; \
				} \
				break; \
			case 0x02: \
				/* semi transparency */ \
				switch( n_tp ) \
				{ \
				case 0: \
					/* 4 bit clut */ \
					TEXTUREWINDOWINTERLEAVED4BIT( TXV, TXU ) \
					TRANSPARENTPIXEL( PIXELUPDATE ) \
					break; \
				case 1: \
					/* 8 bit clut */ \
					TEXTUREWINDOWINTERLEAVED8BIT( TXV, TXU ) \
					TRANSPARENTPIXEL( PIXELUPDATE ) \
					break; \
				case 2: \
					/* 15 bit */ \
					TEXTUREWINDOWINTERLEAVED15BIT( TXV, TXU ) \
					TRANSPARENTPIXEL( PIXELUPDATE ) \
					break; \
				} \
				break; \
			} \
		} \
		else \
		{ \
			/* no texture window */ \
			switch( n_cmd & 0x02 ) \
			{ \
			case 0x00: \
				/* shading */ \
				switch( n_tp ) \
				{ \
				case 0: \
					/* 4 bit clut */ \
					TEXTUREINTERLEAVED4BIT( TXV, TXU ) \
					SHADEDPIXEL( PIXELUPDATE ) \
					break; \
				case 1: \
					/* 8 bit clut */ \
					TEXTUREINTERLEAVED8BIT( TXV, TXU ) \
					SHADEDPIXEL( PIXELUPDATE ) \
					break; \
				case 2: \
					/* 15 bit */ \
					TEXTUREINTERLEAVED15BIT( TXV, TXU ) \
					SHADEDPIXEL( PIXELUPDATE ) \
					break; \
				} \
				break; \
			case 0x02: \
				/* semi transparency */ \
				switch( n_tp ) \
				{ \
				case 0: \
					/* 4 bit clut */ \
					TEXTUREINTERLEAVED4BIT( TXV, TXU ) \
					TRANSPARENTPIXEL( PIXELUPDATE ) \
					break; \
				case 1: \
					/* 8 bit clut */ \
					TEXTUREINTERLEAVED8BIT( TXV, TXU ) \
					TRANSPARENTPIXEL( PIXELUPDATE ) \
					break; \
				case 2: \
					/* 15 bit */ \
					TEXTUREINTERLEAVED15BIT( TXV, TXU ) \
					TRANSPARENTPIXEL( PIXELUPDATE ) \
					break; \
				} \
				break; \
			} \
		} \
	} \
	else \
	{ \
		/* standard texture */ \
		if( m_n_twh != 256 || \
			m_n_tww != 256 || \
			m_n_twx != 0 || \
			m_n_twy != 0 ) \
		{ \
			/* texture window */ \
			switch( n_cmd & 0x02 ) \
			{ \
			case 0x00: \
				/* shading */ \
				switch( n_tp ) \
				{ \
				case 0: \
					/* 4 bit clut */ \
					TEXTUREWINDOW4BIT( TXV, TXU ) \
					SHADEDPIXEL( PIXELUPDATE ) \
					break; \
				case 1: \
					/* 8 bit clut */ \
					TEXTUREWINDOW8BIT( TXV, TXU ) \
					SHADEDPIXEL( PIXELUPDATE ) \
					break; \
				case 2: \
					/* 15 bit */ \
					TEXTUREWINDOW15BIT( TXV, TXU ) \
					SHADEDPIXEL( PIXELUPDATE ) \
					break; \
				} \
				break; \
			case 0x02: \
				/* semi transparency */ \
				switch( n_tp ) \
				{ \
				case 0: \
					/* 4 bit clut */ \
					TEXTUREWINDOW4BIT( TXV, TXU ) \
					TRANSPARENTPIXEL( PIXELUPDATE ) \
					break; \
				case 1: \
					/* 8 bit clut */ \
					TEXTUREWINDOW8BIT( TXV, TXU ) \
					TRANSPARENTPIXEL( PIXELUPDATE ) \
					break; \
				case 2: \
					/* 15 bit */ \
					TEXTUREWINDOW15BIT( TXV, TXU ) \
					TRANSPARENTPIXEL( PIXELUPDATE ) \
					break; \
				} \
				break; \
			} \
		} \
		else \
		{ \
			/* no texture window */ \
			switch( n_cmd & 0x02 ) \
			{ \
			case 0x00: \
				/* shading */ \
				switch( n_tp ) \
				{ \
				case 0: \
					TEXTURE4BIT( TXV, TXU ) \
					SHADEDPIXEL( PIXELUPDATE ) \
					break; \
				case 1: \
					/* 8 bit clut */ \
					TEXTURE8BIT( TXV, TXU ) \
					SHADEDPIXEL( PIXELUPDATE ) \
					break; \
				case 2: \
					/* 15 bit */ \
					TEXTURE15BIT( TXV, TXU ) \
					SHADEDPIXEL( PIXELUPDATE ) \
					break; \
				} \
				break; \
			case 0x02: \
				/* semi transparency */ \
				switch( n_tp ) \
				{ \
				case 0: \
					/* 4 bit clut */ \
					TEXTURE4BIT( TXV, TXU ) \
					TRANSPARENTPIXEL( PIXELUPDATE ) \
					break; \
				case 1: \
					/* 8 bit clut */ \
					TEXTURE8BIT( TXV, TXU ) \
					TRANSPARENTPIXEL( PIXELUPDATE ) \
					break; \
				case 2: \
					/* 15 bit */ \
					TEXTURE15BIT( TXV, TXU ) \
					TRANSPARENTPIXEL( PIXELUPDATE ) \
					break; \
				} \
				break; \
			} \
		} \
	}

static void FlatPolygon( int n_points )
{
	int16_t n_y;
	int16_t n_x;

	uint32_t n_abr;

	uint16_t *p_n_f;
	uint16_t *p_n_redb;
	uint16_t *p_n_greenb;
	uint16_t *p_n_blueb;
	uint16_t *p_n_redtrans;
	uint16_t *p_n_greentrans;
	uint16_t *p_n_bluetrans;

	PAIR n_r;
	PAIR n_g;
	PAIR n_b;
	PAIR n_cx1;
	PAIR n_cx2;
	int32_t n_dx1;
	int32_t n_dx2;

	uint8_t n_cmd;

	int32_t n_distance;

	uint16_t n_point;
	uint16_t n_rightpoint;
	uint16_t n_leftpoint;
	uint16_t *p_n_rightpointlist;
	uint16_t *p_n_leftpointlist;

	uint16_t *p_vram;

#if defined( MAME_DEBUG )
	if( m_n_debugskip == 1 )
	{
		return;
	}
	for( n_point = 0; n_point < n_points; n_point++ )
	{
		DebugMesh( COORD_X( m_packet.FlatPolygon.vertex[ n_point ].n_coord ) + m_n_drawoffset_x, COORD_Y( m_packet.FlatPolygon.vertex[ n_point ].n_coord ) + m_n_drawoffset_y );
	}
	DebugMeshEnd();
#endif

	n_cmd = BGR_C( m_packet.FlatPolygon.n_bgr );

	SOLIDSETUP( m_n_drawmode )

	n_r.w.h = BGR_R( m_packet.FlatPolygon.n_bgr ); n_r.w.l = 0;
	n_g.w.h = BGR_G( m_packet.FlatPolygon.n_bgr ); n_g.w.l = 0;
	n_b.w.h = BGR_B( m_packet.FlatPolygon.n_bgr ); n_b.w.l = 0;

	FINDTOPLEFT( FlatPolygon )

	n_dx1 = 0;
	n_dx2 = 0;

	n_y = COORD_Y( m_packet.FlatPolygon.vertex[ n_rightpoint ].n_coord );

	for( ;; )
	{
		if( n_y == COORD_Y( m_packet.FlatPolygon.vertex[ n_leftpoint ].n_coord ) )
		{
			while( n_y == COORD_Y( m_packet.FlatPolygon.vertex[ p_n_leftpointlist[ n_leftpoint ] ].n_coord ) )
			{
				n_leftpoint = p_n_leftpointlist[ n_leftpoint ];
				if( n_leftpoint == n_rightpoint )
				{
					break;
				}
			}
			n_cx1.w.h = COORD_X( m_packet.FlatPolygon.vertex[ n_leftpoint ].n_coord ); n_cx1.w.l = 0;
			n_leftpoint = p_n_leftpointlist[ n_leftpoint ];
			n_distance = COORD_Y( m_packet.FlatPolygon.vertex[ n_leftpoint ].n_coord ) - n_y;
			if( n_distance < 1 )
			{
				break;
			}
			n_dx1 = (int32_t)( ( COORD_X( m_packet.FlatPolygon.vertex[ n_leftpoint ].n_coord ) << 16 ) - n_cx1.d ) / n_distance;
		}
		if( n_y == COORD_Y( m_packet.FlatPolygon.vertex[ n_rightpoint ].n_coord ) )
		{
			while( n_y == COORD_Y( m_packet.FlatPolygon.vertex[ p_n_rightpointlist[ n_rightpoint ] ].n_coord ) )
			{
				n_rightpoint = p_n_rightpointlist[ n_rightpoint ];
				if( n_rightpoint == n_leftpoint )
				{
					break;
				}
			}
			n_cx2.w.h = COORD_X( m_packet.FlatPolygon.vertex[ n_rightpoint ].n_coord ); n_cx2.w.l = 0;
			n_rightpoint = p_n_rightpointlist[ n_rightpoint ];
			n_distance = COORD_Y( m_packet.FlatPolygon.vertex[ n_rightpoint ].n_coord ) - n_y;
			if( n_distance < 1 )
			{
				break;
			}
			n_dx2 = (int32_t)( ( COORD_X( m_packet.FlatPolygon.vertex[ n_rightpoint ].n_coord ) << 16 ) - n_cx2.d ) / n_distance;
		}
		if( (int16_t)n_cx1.w.h != (int16_t)n_cx2.w.h && n_y >= (int32_t)m_n_drawarea_y1 && n_y <= (int32_t)m_n_drawarea_y2 )
		{
			if( (int16_t)n_cx1.w.h < (int16_t)n_cx2.w.h )
			{
				n_x = n_cx1.w.h;
				n_distance = (int16_t)n_cx2.w.h - n_x;
			}
			else
			{
				n_x = n_cx2.w.h;
				n_distance = (int16_t)n_cx1.w.h - n_x;
			}

			if( ( (int32_t)m_n_drawarea_x1 - n_x ) > 0 )
			{
				n_distance -= ( m_n_drawarea_x1 - n_x );
				n_x = m_n_drawarea_x1;
			}
			SOLIDFILL( FLATPOLYGONUPDATE )
		}
		n_cx1.d += n_dx1;
		n_cx2.d += n_dx2;
		n_y++;
	}
}

static void FlatTexturedPolygon( int n_points )
{
	int16_t n_y;
	int16_t n_x;

	uint8_t n_cmd;

	uint32_t n_clutx;
	uint32_t n_cluty;

	uint32_t n_tp;
	uint32_t n_tx;
	uint32_t n_ty;
	uint32_t n_abr;
	uint32_t n_ti;

	uint16_t *p_n_f;
	uint16_t *p_n_redb;
	uint16_t *p_n_greenb;
	uint16_t *p_n_blueb;
	uint16_t *p_n_redtrans;
	uint16_t *p_n_greentrans;
	uint16_t *p_n_bluetrans;

	PAIR n_r;
	PAIR n_g;
	PAIR n_b;
	PAIR n_u;
	PAIR n_v;

	PAIR n_cx1;
	PAIR n_cx2;
	PAIR n_cu1;
	PAIR n_cv1;
	PAIR n_cu2;
	PAIR n_cv2;
	int32_t n_du;
	int32_t n_dv;
	int32_t n_dx1;
	int32_t n_dx2;
	int32_t n_du1;
	int32_t n_dv1;
	int32_t n_du2;
	int32_t n_dv2;

	int32_t n_distance;
	uint16_t n_point;
	uint16_t n_rightpoint;
	uint16_t n_leftpoint;
	uint16_t *p_n_rightpointlist;
	uint16_t *p_n_leftpointlist;
	uint16_t *p_clut;
	uint16_t *p_vram;
	uint32_t n_bgr;

#if defined( MAME_DEBUG )
	if( m_n_debugskip == 2 )
	{
		return;
	}
	for( n_point = 0; n_point < n_points; n_point++ )
	{
		DebugMesh( COORD_X( m_packet.FlatTexturedPolygon.vertex[ n_point ].n_coord ) + m_n_drawoffset_x, COORD_Y( m_packet.FlatTexturedPolygon.vertex[ n_point ].n_coord ) + m_n_drawoffset_y );
	}
	DebugMeshEnd();
#endif

	n_cmd = BGR_C( m_packet.FlatTexturedPolygon.n_bgr );

	n_clutx = ( m_packet.FlatTexturedPolygon.vertex[ 0 ].n_texture.w.h & 0x3f ) << 4;
	n_cluty = ( m_packet.FlatTexturedPolygon.vertex[ 0 ].n_texture.w.h >> 6 ) & 0x3ff;

	TEXTURESETUP( m_packet.FlatTexturedPolygon.vertex[ 1 ].n_texture.w.h )

	switch( n_cmd & 0x01 )
	{
	case 0:
		n_r.w.h = BGR_R( m_packet.FlatTexturedPolygon.n_bgr ); n_r.w.l = 0;
		n_g.w.h = BGR_G( m_packet.FlatTexturedPolygon.n_bgr ); n_g.w.l = 0;
		n_b.w.h = BGR_B( m_packet.FlatTexturedPolygon.n_bgr ); n_b.w.l = 0;
		break;
	case 1:
		n_r.w.h = 0x80; n_r.w.l = 0;
		n_g.w.h = 0x80; n_g.w.l = 0;
		n_b.w.h = 0x80; n_b.w.l = 0;
		break;
	}

	FINDTOPLEFT( FlatTexturedPolygon )

	n_dx1 = 0;
	n_dx2 = 0;
	n_du1 = 0;
	n_du2 = 0;
	n_dv1 = 0;
	n_dv2 = 0;

	n_y = COORD_Y( m_packet.FlatTexturedPolygon.vertex[ n_rightpoint ].n_coord );

	for( ;; )
	{
		if( n_y == COORD_Y( m_packet.FlatTexturedPolygon.vertex[ n_leftpoint ].n_coord ) )
		{
			while( n_y == COORD_Y( m_packet.FlatTexturedPolygon.vertex[ p_n_leftpointlist[ n_leftpoint ] ].n_coord ) )
			{
				n_leftpoint = p_n_leftpointlist[ n_leftpoint ];
				if( n_leftpoint == n_rightpoint )
				{
					break;
				}
			}
			n_cx1.w.h = COORD_X( m_packet.FlatTexturedPolygon.vertex[ n_leftpoint ].n_coord ); n_cx1.w.l = 0;
			n_cu1.w.h = TEXTURE_U( m_packet.FlatTexturedPolygon.vertex[ n_leftpoint ].n_texture ); n_cu1.w.l = 0;
			n_cv1.w.h = TEXTURE_V( m_packet.FlatTexturedPolygon.vertex[ n_leftpoint ].n_texture ); n_cv1.w.l = 0;
			n_leftpoint = p_n_leftpointlist[ n_leftpoint ];
			n_distance = COORD_Y( m_packet.FlatTexturedPolygon.vertex[ n_leftpoint ].n_coord ) - n_y;
			if( n_distance < 1 )
			{
				break;
			}
			n_dx1 = (int32_t)( ( COORD_X( m_packet.FlatTexturedPolygon.vertex[ n_leftpoint ].n_coord ) << 16 ) - n_cx1.d ) / n_distance;
			n_du1 = (int32_t)( ( TEXTURE_U( m_packet.FlatTexturedPolygon.vertex[ n_leftpoint ].n_texture ) << 16 ) - n_cu1.d ) / n_distance;
			n_dv1 = (int32_t)( ( TEXTURE_V( m_packet.FlatTexturedPolygon.vertex[ n_leftpoint ].n_texture ) << 16 ) - n_cv1.d ) / n_distance;
		}
		if( n_y == COORD_Y( m_packet.FlatTexturedPolygon.vertex[ n_rightpoint ].n_coord ) )
		{
			while( n_y == COORD_Y( m_packet.FlatTexturedPolygon.vertex[ p_n_rightpointlist[ n_rightpoint ] ].n_coord ) )
			{
				n_rightpoint = p_n_rightpointlist[ n_rightpoint ];
				if( n_rightpoint == n_leftpoint )
				{
					break;
				}
			}
			n_cx2.w.h = COORD_X( m_packet.FlatTexturedPolygon.vertex[ n_rightpoint ].n_coord ); n_cx2.w.l = 0;
			n_cu2.w.h = TEXTURE_U( m_packet.FlatTexturedPolygon.vertex[ n_rightpoint ].n_texture ); n_cu2.w.l = 0;
			n_cv2.w.h = TEXTURE_V( m_packet.FlatTexturedPolygon.vertex[ n_rightpoint ].n_texture ); n_cv2.w.l = 0;
			n_rightpoint = p_n_rightpointlist[ n_rightpoint ];
			n_distance = COORD_Y( m_packet.FlatTexturedPolygon.vertex[ n_rightpoint ].n_coord ) - n_y;
			if( n_distance < 1 )
			{
				break;
			}
			n_dx2 = (int32_t)( ( COORD_X( m_packet.FlatTexturedPolygon.vertex[ n_rightpoint ].n_coord ) << 16 ) - n_cx2.d ) / n_distance;
			n_du2 = (int32_t)( ( TEXTURE_U( m_packet.FlatTexturedPolygon.vertex[ n_rightpoint ].n_texture ) << 16 ) - n_cu2.d ) / n_distance;
			n_dv2 = (int32_t)( ( TEXTURE_V( m_packet.FlatTexturedPolygon.vertex[ n_rightpoint ].n_texture ) << 16 ) - n_cv2.d ) / n_distance;
		}
		if( (int16_t)n_cx1.w.h != (int16_t)n_cx2.w.h && n_y >= (int32_t)m_n_drawarea_y1 && n_y <= (int32_t)m_n_drawarea_y2 )
		{
			if( (int16_t)n_cx1.w.h < (int16_t)n_cx2.w.h )
			{
				n_x = n_cx1.w.h;
				n_distance = (int16_t)n_cx2.w.h - n_x;

				n_u.d = n_cu1.d;
				n_v.d = n_cv1.d;
				n_du = (int32_t)( n_cu2.d - n_cu1.d ) / n_distance;
				n_dv = (int32_t)( n_cv2.d - n_cv1.d ) / n_distance;
			}
			else
			{
				n_x = n_cx2.w.h;
				n_distance = (int16_t)n_cx1.w.h - n_x;

				n_u.d = n_cu2.d;
				n_v.d = n_cv2.d;
				n_du = (int32_t)( n_cu1.d - n_cu2.d ) / n_distance;
				n_dv = (int32_t)( n_cv1.d - n_cv2.d ) / n_distance;
			}

			if( ( (int32_t)m_n_drawarea_x1 - n_x ) > 0 )
			{
				n_u.d += n_du * ( m_n_drawarea_x1 - n_x );
				n_v.d += n_dv * ( m_n_drawarea_x1 - n_x );
				n_distance -= ( m_n_drawarea_x1 - n_x );
				n_x = m_n_drawarea_x1;
			}
			TEXTUREFILL( FLATTEXTUREDPOLYGONUPDATE, n_u.w.h, n_v.w.h );
		}
		n_cx1.d += n_dx1;
		n_cu1.d += n_du1;
		n_cv1.d += n_dv1;
		n_cx2.d += n_dx2;
		n_cu2.d += n_du2;
		n_cv2.d += n_dv2;
		n_y++;
	}
}

static void GouraudPolygon( int n_points )
{
	int16_t n_y;
	int16_t n_x;

	uint32_t n_abr;

	uint16_t *p_n_f;
	uint16_t *p_n_redb;
	uint16_t *p_n_greenb;
	uint16_t *p_n_blueb;
	uint16_t *p_n_redtrans;
	uint16_t *p_n_greentrans;
	uint16_t *p_n_bluetrans;

	uint8_t n_cmd;

	PAIR n_r;
	PAIR n_g;
	PAIR n_b;
	PAIR n_cx1;
	PAIR n_cx2;
	PAIR n_cr1;
	PAIR n_cg1;
	PAIR n_cb1;
	PAIR n_cr2;
	PAIR n_cg2;
	PAIR n_cb2;
	int32_t n_dr;
	int32_t n_dg;
	int32_t n_db;
	int32_t n_dx1;
	int32_t n_dx2;
	int32_t n_dr1;
	int32_t n_dg1;
	int32_t n_db1;
	int32_t n_dr2;
	int32_t n_dg2;
	int32_t n_db2;

	int32_t n_distance;

	uint16_t n_point;
	uint16_t n_rightpoint;
	uint16_t n_leftpoint;
	uint16_t *p_n_rightpointlist;
	uint16_t *p_n_leftpointlist;

	uint16_t *p_vram;

#if defined( MAME_DEBUG )
	if( m_n_debugskip == 3 )
	{
		return;
	}
	for( n_point = 0; n_point < n_points; n_point++ )
	{
		DebugMesh( COORD_X( m_packet.GouraudPolygon.vertex[ n_point ].n_coord ) + m_n_drawoffset_x, COORD_Y( m_packet.GouraudPolygon.vertex[ n_point ].n_coord ) + m_n_drawoffset_y );
	}
	DebugMeshEnd();
#endif

	n_cmd = BGR_C( m_packet.GouraudPolygon.vertex[ 0 ].n_bgr );

	SOLIDSETUP( m_n_drawmode )

	FINDTOPLEFT( GouraudPolygon )

	n_dx1 = 0;
	n_dx2 = 0;
	n_dr1 = 0;
	n_dr2 = 0;
	n_dg1 = 0;
	n_dg2 = 0;
	n_db1 = 0;
	n_db2 = 0;

	n_y = COORD_Y( m_packet.GouraudPolygon.vertex[ n_rightpoint ].n_coord );

	for( ;; )
	{
		if( n_y == COORD_Y( m_packet.GouraudPolygon.vertex[ n_leftpoint ].n_coord ) )
		{
			while( n_y == COORD_Y( m_packet.GouraudPolygon.vertex[ p_n_leftpointlist[ n_leftpoint ] ].n_coord ) )
			{
				n_leftpoint = p_n_leftpointlist[ n_leftpoint ];
				if( n_leftpoint == n_rightpoint )
				{
					break;
				}
			}
			n_cx1.w.h = COORD_X( m_packet.GouraudPolygon.vertex[ n_leftpoint ].n_coord ); n_cx1.w.l = 0;
			n_cr1.w.h = BGR_R( m_packet.GouraudPolygon.vertex[ n_leftpoint ].n_bgr ); n_cr1.w.l = 0;
			n_cg1.w.h = BGR_G( m_packet.GouraudPolygon.vertex[ n_leftpoint ].n_bgr ); n_cg1.w.l = 0;
			n_cb1.w.h = BGR_B( m_packet.GouraudPolygon.vertex[ n_leftpoint ].n_bgr ); n_cb1.w.l = 0;
			n_leftpoint = p_n_leftpointlist[ n_leftpoint ];
			n_distance = COORD_Y( m_packet.GouraudPolygon.vertex[ n_leftpoint ].n_coord ) - n_y;
			if( n_distance < 1 )
			{
				break;
			}
			n_dx1 = (int32_t)( ( COORD_X( m_packet.GouraudPolygon.vertex[ n_leftpoint ].n_coord ) << 16 ) - n_cx1.d ) / n_distance;
			n_dr1 = (int32_t)( ( BGR_R( m_packet.GouraudPolygon.vertex[ n_leftpoint ].n_bgr ) << 16 ) - n_cr1.d ) / n_distance;
			n_dg1 = (int32_t)( ( BGR_G( m_packet.GouraudPolygon.vertex[ n_leftpoint ].n_bgr ) << 16 ) - n_cg1.d ) / n_distance;
			n_db1 = (int32_t)( ( BGR_B( m_packet.GouraudPolygon.vertex[ n_leftpoint ].n_bgr ) << 16 ) - n_cb1.d ) / n_distance;
		}
		if( n_y == COORD_Y( m_packet.GouraudPolygon.vertex[ n_rightpoint ].n_coord ) )
		{
			while( n_y == COORD_Y( m_packet.GouraudPolygon.vertex[ p_n_rightpointlist[ n_rightpoint ] ].n_coord ) )
			{
				n_rightpoint = p_n_rightpointlist[ n_rightpoint ];
				if( n_rightpoint == n_leftpoint )
				{
					break;
				}
			}
			n_cx2.w.h = COORD_X( m_packet.GouraudPolygon.vertex[ n_rightpoint ].n_coord ); n_cx2.w.l = 0;
			n_cr2.w.h = BGR_R( m_packet.GouraudPolygon.vertex[ n_rightpoint ].n_bgr ); n_cr2.w.l = 0;
			n_cg2.w.h = BGR_G( m_packet.GouraudPolygon.vertex[ n_rightpoint ].n_bgr ); n_cg2.w.l = 0;
			n_cb2.w.h = BGR_B( m_packet.GouraudPolygon.vertex[ n_rightpoint ].n_bgr ); n_cb2.w.l = 0;
			n_rightpoint = p_n_rightpointlist[ n_rightpoint ];
			n_distance = COORD_Y( m_packet.GouraudPolygon.vertex[ n_rightpoint ].n_coord ) - n_y;
			if( n_distance < 1 )
			{
				break;
			}
			n_dx2 = (int32_t)( ( COORD_X( m_packet.GouraudPolygon.vertex[ n_rightpoint ].n_coord ) << 16 ) - n_cx2.d ) / n_distance;
			n_dr2 = (int32_t)( ( BGR_R( m_packet.GouraudPolygon.vertex[ n_rightpoint ].n_bgr ) << 16 ) - n_cr2.d ) / n_distance;
			n_dg2 = (int32_t)( ( BGR_G( m_packet.GouraudPolygon.vertex[ n_rightpoint ].n_bgr ) << 16 ) - n_cg2.d ) / n_distance;
			n_db2 = (int32_t)( ( BGR_B( m_packet.GouraudPolygon.vertex[ n_rightpoint ].n_bgr ) << 16 ) - n_cb2.d ) / n_distance;
		}
		if( (int16_t)n_cx1.w.h != (int16_t)n_cx2.w.h && n_y >= (int32_t)m_n_drawarea_y1 && n_y <= (int32_t)m_n_drawarea_y2 )
		{
			if( (int16_t)n_cx1.w.h < (int16_t)n_cx2.w.h )
			{
				n_x = n_cx1.w.h;
				n_distance = (int16_t)n_cx2.w.h - n_x;

				n_r.d = n_cr1.d;
				n_g.d = n_cg1.d;
				n_b.d = n_cb1.d;
				n_dr = (int32_t)( n_cr2.d - n_cr1.d ) / n_distance;
				n_dg = (int32_t)( n_cg2.d - n_cg1.d ) / n_distance;
				n_db = (int32_t)( n_cb2.d - n_cb1.d ) / n_distance;
			}
			else
			{
				n_x = n_cx2.w.h;
				n_distance = (int16_t)n_cx1.w.h - n_x;

				n_r.d = n_cr2.d;
				n_g.d = n_cg2.d;
				n_b.d = n_cb2.d;
				n_dr = (int32_t)( n_cr1.d - n_cr2.d ) / n_distance;
				n_dg = (int32_t)( n_cg1.d - n_cg2.d ) / n_distance;
				n_db = (int32_t)( n_cb1.d - n_cb2.d ) / n_distance;
			}

			if( ( (int32_t)m_n_drawarea_x1 - n_x ) > 0 )
			{
				n_r.d += n_dr * ( m_n_drawarea_x1 - n_x );
				n_g.d += n_dg * ( m_n_drawarea_x1 - n_x );
				n_b.d += n_db * ( m_n_drawarea_x1 - n_x );
				n_distance -= ( m_n_drawarea_x1 - n_x );
				n_x = m_n_drawarea_x1;
			}
			SOLIDFILL( GOURAUDPOLYGONUPDATE )
		}
		n_cx1.d += n_dx1;
		n_cr1.d += n_dr1;
		n_cg1.d += n_dg1;
		n_cb1.d += n_db1;
		n_cx2.d += n_dx2;
		n_cr2.d += n_dr2;
		n_cg2.d += n_dg2;
		n_cb2.d += n_db2;
		n_y++;
	}
}

static void GouraudTexturedPolygon( int n_points )
{
	int16_t n_y;
	int16_t n_x;

	uint8_t n_cmd;

	uint32_t n_clutx;
	uint32_t n_cluty;

	uint32_t n_tp;
	uint32_t n_tx;
	uint32_t n_ty;
	uint32_t n_abr;
	uint32_t n_ti;

	uint16_t *p_n_f;
	uint16_t *p_n_redb;
	uint16_t *p_n_greenb;
	uint16_t *p_n_blueb;
	uint16_t *p_n_redtrans;
	uint16_t *p_n_greentrans;
	uint16_t *p_n_bluetrans;

	PAIR n_r;
	PAIR n_g;
	PAIR n_b;
	PAIR n_u;
	PAIR n_v;

	PAIR n_cx1;
	PAIR n_cx2;
	PAIR n_cu1;
	PAIR n_cv1;
	PAIR n_cu2;
	PAIR n_cv2;
	PAIR n_cr1;
	PAIR n_cg1;
	PAIR n_cb1;
	PAIR n_cr2;
	PAIR n_cg2;
	PAIR n_cb2;
	int32_t n_dr;
	int32_t n_dg;
	int32_t n_db;
	int32_t n_du;
	int32_t n_dv;
	int32_t n_dx1;
	int32_t n_dx2;
	int32_t n_dr1;
	int32_t n_dg1;
	int32_t n_db1;
	int32_t n_dr2;
	int32_t n_dg2;
	int32_t n_db2;
	int32_t n_du1;
	int32_t n_dv1;
	int32_t n_du2;
	int32_t n_dv2;

	int32_t n_distance;
	uint16_t n_point;
	uint16_t n_rightpoint;
	uint16_t n_leftpoint;
	uint16_t *p_n_rightpointlist;
	uint16_t *p_n_leftpointlist;
	uint16_t *p_clut;
	uint16_t *p_vram;
	uint32_t n_bgr;

#if defined( MAME_DEBUG )
	if( m_n_debugskip == 4 )
	{
		return;
	}
	for( n_point = 0; n_point < n_points; n_point++ )
	{
		DebugMesh( COORD_X( m_packet.GouraudTexturedPolygon.vertex[ n_point ].n_coord ) + m_n_drawoffset_x, COORD_Y( m_packet.GouraudTexturedPolygon.vertex[ n_point ].n_coord ) + m_n_drawoffset_y );
	}
	DebugMeshEnd();
#endif

	n_cmd = BGR_C( m_packet.GouraudTexturedPolygon.vertex[ 0 ].n_bgr );

	n_clutx = ( m_packet.GouraudTexturedPolygon.vertex[ 0 ].n_texture.w.h & 0x3f ) << 4;
	n_cluty = ( m_packet.GouraudTexturedPolygon.vertex[ 0 ].n_texture.w.h >> 6 ) & 0x3ff;

	TEXTURESETUP( m_packet.GouraudTexturedPolygon.vertex[ 1 ].n_texture.w.h )

	FINDTOPLEFT( GouraudTexturedPolygon )

	n_dx1 = 0;
	n_dx2 = 0;
	n_du1 = 0;
	n_du2 = 0;
	n_dr1 = 0;
	n_dr2 = 0;
	n_dg1 = 0;
	n_dg2 = 0;
	n_db1 = 0;
	n_db2 = 0;
	n_dv1 = 0;
	n_dv2 = 0;

	n_y = COORD_Y( m_packet.GouraudTexturedPolygon.vertex[ n_rightpoint ].n_coord );

	for( ;; )
	{
		if( n_y == COORD_Y( m_packet.GouraudTexturedPolygon.vertex[ n_leftpoint ].n_coord ) )
		{
			while( n_y == COORD_Y( m_packet.GouraudTexturedPolygon.vertex[ p_n_leftpointlist[ n_leftpoint ] ].n_coord ) )
			{
				n_leftpoint = p_n_leftpointlist[ n_leftpoint ];
				if( n_leftpoint == n_rightpoint )
				{
					break;
				}
			}
			n_cx1.w.h = COORD_X( m_packet.GouraudTexturedPolygon.vertex[ n_leftpoint ].n_coord ); n_cx1.w.l = 0;
			switch( n_cmd & 0x01 )
			{
			case 0x00:
				n_cr1.w.h = BGR_R( m_packet.GouraudTexturedPolygon.vertex[ n_leftpoint ].n_bgr ); n_cr1.w.l = 0;
				n_cg1.w.h = BGR_G( m_packet.GouraudTexturedPolygon.vertex[ n_leftpoint ].n_bgr ); n_cg1.w.l = 0;
				n_cb1.w.h = BGR_B( m_packet.GouraudTexturedPolygon.vertex[ n_leftpoint ].n_bgr ); n_cb1.w.l = 0;
				break;
			case 0x01:
				n_cr1.w.h = 0x80; n_cr1.w.l = 0;
				n_cg1.w.h = 0x80; n_cg1.w.l = 0;
				n_cb1.w.h = 0x80; n_cb1.w.l = 0;
				break;
			}
			n_cu1.w.h = TEXTURE_U( m_packet.GouraudTexturedPolygon.vertex[ n_leftpoint ].n_texture ); n_cu1.w.l = 0;
			n_cv1.w.h = TEXTURE_V( m_packet.GouraudTexturedPolygon.vertex[ n_leftpoint ].n_texture ); n_cv1.w.l = 0;
			n_leftpoint = p_n_leftpointlist[ n_leftpoint ];
			n_distance = COORD_Y( m_packet.GouraudTexturedPolygon.vertex[ n_leftpoint ].n_coord ) - n_y;
			if( n_distance < 1 )
			{
				break;
			}
			n_dx1 = (int32_t)( ( COORD_X( m_packet.GouraudTexturedPolygon.vertex[ n_leftpoint ].n_coord ) << 16 ) - n_cx1.d ) / n_distance;
			switch( n_cmd & 0x01 )
			{
			case 0x00:
				n_dr1 = (int32_t)( ( BGR_R( m_packet.GouraudTexturedPolygon.vertex[ n_leftpoint ].n_bgr ) << 16 ) - n_cr1.d ) / n_distance;
				n_dg1 = (int32_t)( ( BGR_G( m_packet.GouraudTexturedPolygon.vertex[ n_leftpoint ].n_bgr ) << 16 ) - n_cg1.d ) / n_distance;
				n_db1 = (int32_t)( ( BGR_B( m_packet.GouraudTexturedPolygon.vertex[ n_leftpoint ].n_bgr ) << 16 ) - n_cb1.d ) / n_distance;
				break;
			case 0x01:
				n_dr1 = 0;
				n_dg1 = 0;
				n_db1 = 0;
				break;
			}
			n_du1 = (int32_t)( ( TEXTURE_U( m_packet.GouraudTexturedPolygon.vertex[ n_leftpoint ].n_texture ) << 16 ) - n_cu1.d ) / n_distance;
			n_dv1 = (int32_t)( ( TEXTURE_V( m_packet.GouraudTexturedPolygon.vertex[ n_leftpoint ].n_texture ) << 16 ) - n_cv1.d ) / n_distance;
		}
		if( n_y == COORD_Y( m_packet.GouraudTexturedPolygon.vertex[ n_rightpoint ].n_coord ) )
		{
			while( n_y == COORD_Y( m_packet.GouraudTexturedPolygon.vertex[ p_n_rightpointlist[ n_rightpoint ] ].n_coord ) )
			{
				n_rightpoint = p_n_rightpointlist[ n_rightpoint ];
				if( n_rightpoint == n_leftpoint )
				{
					break;
				}
			}
			n_cx2.w.h = COORD_X( m_packet.GouraudTexturedPolygon.vertex[ n_rightpoint ].n_coord ); n_cx2.w.l = 0;
			switch( n_cmd & 0x01 )
			{
			case 0x00:
				n_cr2.w.h = BGR_R( m_packet.GouraudTexturedPolygon.vertex[ n_rightpoint ].n_bgr ); n_cr2.w.l = 0;
				n_cg2.w.h = BGR_G( m_packet.GouraudTexturedPolygon.vertex[ n_rightpoint ].n_bgr ); n_cg2.w.l = 0;
				n_cb2.w.h = BGR_B( m_packet.GouraudTexturedPolygon.vertex[ n_rightpoint ].n_bgr ); n_cb2.w.l = 0;
				break;
			case 0x01:
				n_cr2.w.h = 0x80; n_cr2.w.l = 0;
				n_cg2.w.h = 0x80; n_cg2.w.l = 0;
				n_cb2.w.h = 0x80; n_cb2.w.l = 0;
				break;
			}
			n_cu2.w.h = TEXTURE_U( m_packet.GouraudTexturedPolygon.vertex[ n_rightpoint ].n_texture ); n_cu2.w.l = 0;
			n_cv2.w.h = TEXTURE_V( m_packet.GouraudTexturedPolygon.vertex[ n_rightpoint ].n_texture ); n_cv2.w.l = 0;
			n_rightpoint = p_n_rightpointlist[ n_rightpoint ];
			n_distance = COORD_Y( m_packet.GouraudTexturedPolygon.vertex[ n_rightpoint ].n_coord ) - n_y;
			if( n_distance < 1 )
			{
				break;
			}
			n_dx2 = (int32_t)( ( COORD_X( m_packet.GouraudTexturedPolygon.vertex[ n_rightpoint ].n_coord ) << 16 ) - n_cx2.d ) / n_distance;
			switch( n_cmd & 0x01 )
			{
			case 0x00:
				n_dr2 = (int32_t)( ( BGR_R( m_packet.GouraudTexturedPolygon.vertex[ n_rightpoint ].n_bgr ) << 16 ) - n_cr2.d ) / n_distance;
				n_dg2 = (int32_t)( ( BGR_G( m_packet.GouraudTexturedPolygon.vertex[ n_rightpoint ].n_bgr ) << 16 ) - n_cg2.d ) / n_distance;
				n_db2 = (int32_t)( ( BGR_B( m_packet.GouraudTexturedPolygon.vertex[ n_rightpoint ].n_bgr ) << 16 ) - n_cb2.d ) / n_distance;
				break;
			case 0x01:
				n_dr2 = 0;
				n_dg2 = 0;
				n_db2 = 0;
				break;
			}
			n_du2 = (int32_t)( ( TEXTURE_U( m_packet.GouraudTexturedPolygon.vertex[ n_rightpoint ].n_texture ) << 16 ) - n_cu2.d ) / n_distance;
			n_dv2 = (int32_t)( ( TEXTURE_V( m_packet.GouraudTexturedPolygon.vertex[ n_rightpoint ].n_texture ) << 16 ) - n_cv2.d ) / n_distance;
		}
		if( (int16_t)n_cx1.w.h != (int16_t)n_cx2.w.h && n_y >= (int32_t)m_n_drawarea_y1 && n_y <= (int32_t)m_n_drawarea_y2 )
		{
			if( (int16_t)n_cx1.w.h < (int16_t)n_cx2.w.h )
			{
				n_x = n_cx1.w.h;
				n_distance = (int16_t)n_cx2.w.h - n_x;

				n_r.d = n_cr1.d;
				n_g.d = n_cg1.d;
				n_b.d = n_cb1.d;
				n_u.d = n_cu1.d;
				n_v.d = n_cv1.d;
				n_dr = (int32_t)( n_cr2.d - n_cr1.d ) / n_distance;
				n_dg = (int32_t)( n_cg2.d - n_cg1.d ) / n_distance;
				n_db = (int32_t)( n_cb2.d - n_cb1.d ) / n_distance;
				n_du = (int32_t)( n_cu2.d - n_cu1.d ) / n_distance;
				n_dv = (int32_t)( n_cv2.d - n_cv1.d ) / n_distance;
			}
			else
			{
				n_x = n_cx2.w.h;
				n_distance = (int16_t)n_cx1.w.h - n_x;

				n_r.d = n_cr2.d;
				n_g.d = n_cg2.d;
				n_b.d = n_cb2.d;
				n_u.d = n_cu2.d;
				n_v.d = n_cv2.d;
				n_dr = (int32_t)( n_cr1.d - n_cr2.d ) / n_distance;
				n_dg = (int32_t)( n_cg1.d - n_cg2.d ) / n_distance;
				n_db = (int32_t)( n_cb1.d - n_cb2.d ) / n_distance;
				n_du = (int32_t)( n_cu1.d - n_cu2.d ) / n_distance;
				n_dv = (int32_t)( n_cv1.d - n_cv2.d ) / n_distance;
			}

			if( ( (int32_t)m_n_drawarea_x1 - n_x ) > 0 )
			{
				n_r.d += n_dr * ( m_n_drawarea_x1 - n_x );
				n_g.d += n_dg * ( m_n_drawarea_x1 - n_x );
				n_b.d += n_db * ( m_n_drawarea_x1 - n_x );
				n_u.d += n_du * ( m_n_drawarea_x1 - n_x );
				n_v.d += n_dv * ( m_n_drawarea_x1 - n_x );
				n_distance -= ( m_n_drawarea_x1 - n_x );
				n_x = m_n_drawarea_x1;
			}
			TEXTUREFILL( GOURAUDTEXTUREDPOLYGONUPDATE, n_u.w.h, n_v.w.h );
		}
		n_cx1.d += n_dx1;
		n_cr1.d += n_dr1;
		n_cg1.d += n_dg1;
		n_cb1.d += n_db1;
		n_cu1.d += n_du1;
		n_cv1.d += n_dv1;
		n_cx2.d += n_dx2;
		n_cr2.d += n_dr2;
		n_cg2.d += n_dg2;
		n_cb2.d += n_db2;
		n_cu2.d += n_du2;
		n_cv2.d += n_dv2;
		n_y++;
	}
}

static void MonochromeLine( void )
{
	PAIR n_x;
	PAIR n_y;
	int32_t n_dx;
	int32_t n_dy;
	int32_t n_dr;
	int32_t n_dg;
	int32_t n_db;
	int32_t n_len;
	int32_t n_xlen;
	int32_t n_ylen;
	int32_t n_xstart;
	int32_t n_ystart;
	int32_t n_xend;
	int32_t n_yend;
	uint32_t n_r;
	uint32_t n_g;
	uint32_t n_b;
	uint16_t *p_vram;

#if defined( MAME_DEBUG )
	if( m_n_debugskip == 5 )
	{
		return;
	}
	DebugMesh( COORD_X( m_packet.MonochromeLine.vertex[ 0 ].n_coord ) + m_n_drawoffset_x, COORD_Y( m_packet.MonochromeLine.vertex[ 0 ].n_coord ) + m_n_drawoffset_y );
	DebugMesh( COORD_X( m_packet.MonochromeLine.vertex[ 1 ].n_coord ) + m_n_drawoffset_x, COORD_Y( m_packet.MonochromeLine.vertex[ 1 ].n_coord ) + m_n_drawoffset_y );
	DebugMeshEnd();
#endif

	n_xstart = COORD_X( m_packet.MonochromeLine.vertex[ 0 ].n_coord ) + m_n_drawoffset_x;
	n_xend = COORD_X( m_packet.MonochromeLine.vertex[ 1 ].n_coord ) + m_n_drawoffset_x;
	n_ystart = COORD_Y( m_packet.MonochromeLine.vertex[ 0 ].n_coord ) + m_n_drawoffset_y;
	n_yend = COORD_Y( m_packet.MonochromeLine.vertex[ 1 ].n_coord ) + m_n_drawoffset_y;

	n_r = BGR_R( m_packet.MonochromeLine.n_bgr );
	n_g = BGR_G( m_packet.MonochromeLine.n_bgr );
	n_b = BGR_B( m_packet.MonochromeLine.n_bgr );

	if( n_xend > n_xstart )
	{
		n_xlen = n_xend - n_xstart;
	}
	else
	{
		n_xlen = n_xstart - n_xend;
	}

	if( n_yend > n_ystart )
	{
		n_ylen = n_yend - n_ystart;
	}
	else
	{
		n_ylen = n_ystart - n_yend;
	}

	if( n_xlen > n_ylen )
	{
		n_len = n_xlen;
	}
	else
	{
		n_len = n_ylen;
	}

	if( n_len == 0 )
	{
		n_len = 1;
	}

	n_x.w.h = n_xstart; n_x.w.l = 0;
	n_y.w.h = n_ystart; n_y.w.l = 0;

	n_dx = (int32_t)( ( n_xend << 16 ) - n_x.d ) / n_len;
	n_dy = (int32_t)( ( n_yend << 16 ) - n_y.d ) / n_len;
	n_dr = 0;
	n_dg = 0;
	n_db = 0;

	while( n_len > 0 )
	{
		if( (int16_t)n_x.w.h >= (int32_t)m_n_drawarea_x1 &&
			(int16_t)n_y.w.h >= (int32_t)m_n_drawarea_y1 &&
			(int16_t)n_x.w.h <= (int32_t)m_n_drawarea_x2 &&
			(int16_t)n_y.w.h <= (int32_t)m_n_drawarea_y2 )
		{
			p_vram = m_p_p_vram[ n_y.w.h ] + n_x.w.h;
			*( p_vram ) = m_p_n_redshade[ MID_LEVEL | n_r ] |
				m_p_n_greenshade[ MID_LEVEL | n_g ] |
				m_p_n_blueshade[ MID_LEVEL | n_b ];
		}
		n_x.d += n_dx;
		n_y.d += n_dy;
		n_len--;
	}
}

static void GouraudLine( void )
{
	PAIR n_x;
	PAIR n_y;
	int32_t n_dx;
	int32_t n_dy;
	int32_t n_dr;
	int32_t n_dg;
	int32_t n_db;
	int32_t n_distance;
	int32_t n_xlen;
	int32_t n_ylen;
	int32_t n_xstart;
	int32_t n_ystart;
	int32_t n_xend;
	int32_t n_yend;
	PAIR n_r;
	PAIR n_g;
	PAIR n_b;
	PAIR n_cr1;
	PAIR n_cg1;
	PAIR n_cb1;
	PAIR n_cr2;
	PAIR n_cg2;
	PAIR n_cb2;
	uint16_t *p_vram;

#if defined( MAME_DEBUG )
	if( m_n_debugskip == 6 )
	{
		return;
	}
	DebugMesh( COORD_X( m_packet.GouraudLine.vertex[ 0 ].n_coord ) + m_n_drawoffset_x, COORD_Y( m_packet.GouraudLine.vertex[ 0 ].n_coord ) + m_n_drawoffset_y );
	DebugMesh( COORD_X( m_packet.GouraudLine.vertex[ 1 ].n_coord ) + m_n_drawoffset_x, COORD_Y( m_packet.GouraudLine.vertex[ 1 ].n_coord ) + m_n_drawoffset_y );
	DebugMeshEnd();
#endif

	n_xstart = COORD_X( m_packet.GouraudLine.vertex[ 0 ].n_coord ) + m_n_drawoffset_x;
	n_ystart = COORD_Y( m_packet.GouraudLine.vertex[ 0 ].n_coord ) + m_n_drawoffset_y;
	n_cr1.w.h = BGR_R( m_packet.GouraudLine.vertex[ 0 ].n_bgr ); n_cr1.w.l = 0;
	n_cg1.w.h = BGR_G( m_packet.GouraudLine.vertex[ 0 ].n_bgr ); n_cg1.w.l = 0;
	n_cb1.w.h = BGR_B( m_packet.GouraudLine.vertex[ 0 ].n_bgr ); n_cb1.w.l = 0;

	n_xend = COORD_X( m_packet.GouraudLine.vertex[ 1 ].n_coord ) + m_n_drawoffset_x;
	n_yend = COORD_Y( m_packet.GouraudLine.vertex[ 1 ].n_coord ) + m_n_drawoffset_y;
	n_cr2.w.h = BGR_R( m_packet.GouraudLine.vertex[ 1 ].n_bgr ); n_cr1.w.l = 0;
	n_cg2.w.h = BGR_G( m_packet.GouraudLine.vertex[ 1 ].n_bgr ); n_cg1.w.l = 0;
	n_cb2.w.h = BGR_B( m_packet.GouraudLine.vertex[ 1 ].n_bgr ); n_cb1.w.l = 0;

	n_x.w.h = n_xstart; n_x.w.l = 0;
	n_y.w.h = n_ystart; n_y.w.l = 0;
	n_r.d = n_cr1.d;
	n_g.d = n_cg1.d;
	n_b.d = n_cb1.d;

	if( n_xend > n_xstart )
	{
		n_xlen = n_xend - n_xstart;
	}
	else
	{
		n_xlen = n_xstart - n_xend;
	}

	if( n_yend > n_ystart )
	{
		n_ylen = n_yend - n_ystart;
	}
	else
	{
		n_ylen = n_ystart - n_yend;
	}

	if( n_xlen > n_ylen )
	{
		n_distance = n_xlen;
	}
	else
	{
		n_distance = n_ylen;
	}

	if( n_distance == 0 )
	{
		n_distance = 1;
	}

	n_dx = (int32_t)( ( n_xend << 16 ) - n_x.d ) / n_distance;
	n_dy = (int32_t)( ( n_yend << 16 ) - n_y.d ) / n_distance;
	n_dr = (int32_t)( n_cr2.d - n_cr1.d ) / n_distance;
	n_dg = (int32_t)( n_cg2.d - n_cg1.d ) / n_distance;
	n_db = (int32_t)( n_cb2.d - n_cb1.d ) / n_distance;

	while( n_distance > 0 )
	{
		if( (int16_t)n_x.w.h >= (int32_t)m_n_drawarea_x1 &&
			(int16_t)n_y.w.h >= (int32_t)m_n_drawarea_y1 &&
			(int16_t)n_x.w.h <= (int32_t)m_n_drawarea_x2 &&
			(int16_t)n_y.w.h <= (int32_t)m_n_drawarea_y2 )
		{
			p_vram = m_p_p_vram[ n_y.w.h ] + n_x.w.h;
			*( p_vram ) = m_p_n_redshade[ MID_LEVEL | n_r.w.h ] |
				m_p_n_greenshade[ MID_LEVEL | n_g.w.h ] |
				m_p_n_blueshade[ MID_LEVEL | n_b.w.h ];
		}
		n_x.d += n_dx;
		n_y.d += n_dy;
		n_r.d += n_dr;
		n_g.d += n_dg;
		n_b.d += n_db;
		n_distance--;
	}
}

static void FrameBufferRectangleDraw( void )
{
	PAIR n_r;
	PAIR n_g;
	PAIR n_b;
	int32_t n_distance;
	int32_t n_h;
	int16_t n_y;
	int16_t n_x;

#if defined( MAME_DEBUG )
	if( m_n_debugskip == 7 )
	{
		return;
	}
	DebugMesh( COORD_X( m_packet.FlatRectangle.n_coord ), COORD_Y( m_packet.FlatRectangle.n_coord ) );
	DebugMesh( COORD_X( m_packet.FlatRectangle.n_coord ) + SIZE_W( m_packet.FlatRectangle.n_size ), COORD_Y( m_packet.FlatRectangle.n_coord ) );
	DebugMesh( COORD_X( m_packet.FlatRectangle.n_coord ), COORD_Y( m_packet.FlatRectangle.n_coord ) + SIZE_H( m_packet.FlatRectangle.n_size ) );
	DebugMesh( COORD_X( m_packet.FlatRectangle.n_coord ) + SIZE_W( m_packet.FlatRectangle.n_size ), COORD_Y( m_packet.FlatRectangle.n_coord ) + SIZE_H( m_packet.FlatRectangle.n_size ) );
	DebugMeshEnd();
#endif

	n_r.w.h = BGR_R( m_packet.FlatRectangle.n_bgr ); n_r.w.l = 0;
	n_g.w.h = BGR_G( m_packet.FlatRectangle.n_bgr ); n_g.w.l = 0;
	n_b.w.h = BGR_B( m_packet.FlatRectangle.n_bgr ); n_b.w.l = 0;

	n_y = COORD_Y( m_packet.FlatRectangle.n_coord );
	n_h = SIZE_H( m_packet.FlatRectangle.n_size );

	while( n_h > 0 )
	{
		n_x = COORD_X( m_packet.FlatRectangle.n_coord );

		n_distance = SIZE_W( m_packet.FlatRectangle.n_size );
		while( n_distance > 0 )
		{
			*( m_p_p_vram[ n_y & 1023 ] + ( n_x & 1023 ) ) =
				m_p_n_redshade[ MID_LEVEL | n_r.w.h ] |
				m_p_n_greenshade[ MID_LEVEL | n_g.w.h ] |
				m_p_n_blueshade[ MID_LEVEL | n_b.w.h ];
			n_x++;
			n_distance--;
		}
		n_y++;
		n_h--;
	}
}

static void FlatRectangle( void )
{
	int16_t n_y;
	int16_t n_x;

	uint8_t n_cmd;
	uint32_t n_abr;

	uint16_t *p_n_f;
	uint16_t *p_n_redb;
	uint16_t *p_n_greenb;
	uint16_t *p_n_blueb;
	uint16_t *p_n_redtrans;
	uint16_t *p_n_greentrans;
	uint16_t *p_n_bluetrans;

	PAIR n_r;
	PAIR n_g;
	PAIR n_b;

	int32_t n_distance;
	int32_t n_h;
	uint16_t *p_vram;

#if defined( MAME_DEBUG )
	if( m_n_debugskip == 8 )
	{
		return;
	}
	DebugMesh( COORD_X( m_packet.FlatRectangle.n_coord ) + m_n_drawoffset_x, COORD_Y( m_packet.FlatRectangle.n_coord ) + m_n_drawoffset_y );
	DebugMesh( COORD_X( m_packet.FlatRectangle.n_coord ) + m_n_drawoffset_x + SIZE_W( m_packet.FlatRectangle.n_size ), COORD_Y( m_packet.FlatRectangle.n_coord ) + m_n_drawoffset_y );
	DebugMesh( COORD_X( m_packet.FlatRectangle.n_coord ) + m_n_drawoffset_x, COORD_Y( m_packet.FlatRectangle.n_coord ) + m_n_drawoffset_y + SIZE_H( m_packet.FlatRectangle.n_size ) );
	DebugMesh( COORD_X( m_packet.FlatRectangle.n_coord ) + m_n_drawoffset_x + SIZE_W( m_packet.FlatRectangle.n_size ), COORD_Y( m_packet.FlatRectangle.n_coord ) + m_n_drawoffset_y + SIZE_H( m_packet.FlatRectangle.n_size ) );
	DebugMeshEnd();
#endif

	n_cmd = BGR_C( m_packet.FlatRectangle.n_bgr );

	SOLIDSETUP( m_n_drawmode )

	n_r.w.h = BGR_R( m_packet.FlatRectangle.n_bgr ); n_r.w.l = 0;
	n_g.w.h = BGR_G( m_packet.FlatRectangle.n_bgr ); n_g.w.l = 0;
	n_b.w.h = BGR_B( m_packet.FlatRectangle.n_bgr ); n_b.w.l = 0;

	n_y = COORD_Y( m_packet.FlatRectangle.n_coord ) + m_n_drawoffset_y;
	n_h = SIZE_H( m_packet.FlatRectangle.n_size );

	while( n_h > 0 )
	{
		n_x = COORD_X( m_packet.FlatRectangle.n_coord ) + m_n_drawoffset_x;

		n_distance = SIZE_W( m_packet.FlatRectangle.n_size );
		if( n_distance > 0 && n_y >= (int32_t)m_n_drawarea_y1 && n_y <= (int32_t)m_n_drawarea_y2 )
		{
			if( ( (int32_t)m_n_drawarea_x1 - n_x ) > 0 )
			{
				n_distance -= ( m_n_drawarea_x1 - n_x );
				n_x = m_n_drawarea_x1;
			}
			SOLIDFILL( FLATRECTANGEUPDATE )
		}
		n_y++;
		n_h--;
	}
}

static void FlatTexturedRectangle( void )
{
	int16_t n_y;
	int16_t n_x;

	uint8_t n_cmd;

	uint32_t n_clutx;
	uint32_t n_cluty;

	uint32_t n_tp;
	uint32_t n_tx;
	uint32_t n_ty;
	uint32_t n_abr;
	uint32_t n_ti;

	uint16_t *p_n_f;
	uint16_t *p_n_redb;
	uint16_t *p_n_greenb;
	uint16_t *p_n_blueb;
	uint16_t *p_n_redtrans;
	uint16_t *p_n_greentrans;
	uint16_t *p_n_bluetrans;

	PAIR n_r;
	PAIR n_g;
	PAIR n_b;
	uint8_t n_u;
	uint8_t n_v;

	int16_t n_distance;
	uint32_t n_h;
	uint16_t *p_vram;
	uint16_t *p_clut;
	uint16_t n_bgr;

#if defined( MAME_DEBUG )
	if( m_n_debugskip == 9 )
	{
		return;
	}
	DebugMesh( COORD_X( m_packet.FlatTexturedRectangle.n_coord ) + m_n_drawoffset_x, COORD_Y( m_packet.FlatTexturedRectangle.n_coord ) + m_n_drawoffset_y );
	DebugMesh( COORD_X( m_packet.FlatTexturedRectangle.n_coord ) + m_n_drawoffset_x + SIZE_W( m_packet.FlatTexturedRectangle.n_size ), COORD_Y( m_packet.FlatTexturedRectangle.n_coord ) + m_n_drawoffset_y );
	DebugMesh( COORD_X( m_packet.FlatTexturedRectangle.n_coord ) + m_n_drawoffset_x, COORD_Y( m_packet.FlatTexturedRectangle.n_coord ) + m_n_drawoffset_y + SIZE_H( m_packet.FlatTexturedRectangle.n_size ) );
	DebugMesh( COORD_X( m_packet.FlatTexturedRectangle.n_coord ) + m_n_drawoffset_x + SIZE_W( m_packet.FlatTexturedRectangle.n_size ), COORD_Y( m_packet.FlatTexturedRectangle.n_coord ) + m_n_drawoffset_y + SIZE_H( m_packet.FlatTexturedRectangle.n_size ) );
	DebugMeshEnd();
#endif

	n_cmd = BGR_C( m_packet.FlatTexturedRectangle.n_bgr );

	n_clutx = ( m_packet.FlatTexturedRectangle.n_texture.w.h & 0x3f ) << 4;
	n_cluty = ( m_packet.FlatTexturedRectangle.n_texture.w.h >> 6 ) & 0x3ff;

	TEXTURESETUP( m_n_drawmode )

	switch( n_cmd & 0x01 )
	{
	case 0:
		n_r.w.h = BGR_R( m_packet.FlatTexturedRectangle.n_bgr ); n_r.w.l = 0;
		n_g.w.h = BGR_G( m_packet.FlatTexturedRectangle.n_bgr ); n_g.w.l = 0;
		n_b.w.h = BGR_B( m_packet.FlatTexturedRectangle.n_bgr ); n_b.w.l = 0;
		break;
	case 1:
		n_r.w.h = 0x80; n_r.w.l = 0;
		n_g.w.h = 0x80; n_g.w.l = 0;
		n_b.w.h = 0x80; n_b.w.l = 0;
		break;
	}

	n_v = TEXTURE_V( m_packet.FlatTexturedRectangle.n_texture );
	n_y = COORD_Y( m_packet.FlatTexturedRectangle.n_coord ) + m_n_drawoffset_y;
	n_h = SIZE_H( m_packet.FlatTexturedRectangle.n_size );

	while( n_h > 0 )
	{
		n_x = COORD_X( m_packet.FlatTexturedRectangle.n_coord ) + m_n_drawoffset_x;
		n_u = TEXTURE_U( m_packet.FlatTexturedRectangle.n_texture );

		n_distance = SIZE_W( m_packet.FlatTexturedRectangle.n_size );
		if( n_distance > 0 && n_y >= (int32_t)m_n_drawarea_y1 && n_y <= (int32_t)m_n_drawarea_y2 )
		{
			if( ( (int32_t)m_n_drawarea_x1 - n_x ) > 0 )
			{
				n_u += ( m_n_drawarea_x1 - n_x );
				n_distance -= ( m_n_drawarea_x1 - n_x );
				n_x = m_n_drawarea_x1;
			}
			TEXTUREFILL( FLATTEXTUREDRECTANGLEUPDATE, n_u, n_v );
		}
		n_v++;
		n_y++;
		n_h--;
	}
}

static void Sprite8x8( void )
{
	int16_t n_y;
	int16_t n_x;

	uint8_t n_cmd;

	uint32_t n_clutx;
	uint32_t n_cluty;

	uint32_t n_tp;
	uint32_t n_tx;
	uint32_t n_ty;
	uint32_t n_abr;
	uint32_t n_ti;

	uint16_t *p_n_f;
	uint16_t *p_n_redb;
	uint16_t *p_n_greenb;
	uint16_t *p_n_blueb;
	uint16_t *p_n_redtrans;
	uint16_t *p_n_greentrans;
	uint16_t *p_n_bluetrans;

	PAIR n_r;
	PAIR n_g;
	PAIR n_b;
	uint8_t n_u;
	uint8_t n_v;

	int16_t n_distance;
	uint32_t n_h;
	uint16_t *p_vram;
	uint16_t *p_clut;
	uint16_t n_bgr;

#if defined( MAME_DEBUG )
	if( m_n_debugskip == 10 )
	{
		return;
	}
	DebugMesh( COORD_X( m_packet.Sprite8x8.n_coord ) + m_n_drawoffset_x, COORD_Y( m_packet.Sprite8x8.n_coord ) + m_n_drawoffset_y );
	DebugMesh( COORD_X( m_packet.Sprite8x8.n_coord ) + m_n_drawoffset_x + 7, COORD_Y( m_packet.Sprite8x8.n_coord ) + m_n_drawoffset_y );
	DebugMesh( COORD_X( m_packet.Sprite8x8.n_coord ) + m_n_drawoffset_x, COORD_Y( m_packet.Sprite8x8.n_coord ) + m_n_drawoffset_y + 7 );
	DebugMesh( COORD_X( m_packet.Sprite8x8.n_coord ) + m_n_drawoffset_x + 7, COORD_Y( m_packet.Sprite8x8.n_coord ) + m_n_drawoffset_y + 7 );
	DebugMeshEnd();
#endif

	n_cmd = BGR_C( m_packet.Sprite8x8.n_bgr );

	n_clutx = ( m_packet.Sprite8x8.n_texture.w.h & 0x3f ) << 4;
	n_cluty = ( m_packet.Sprite8x8.n_texture.w.h >> 6 ) & 0x3ff;

	TEXTURESETUP( m_n_drawmode )

	n_r.w.h = BGR_R( m_packet.Sprite8x8.n_bgr ); n_r.w.l = 0;
	n_g.w.h = BGR_G( m_packet.Sprite8x8.n_bgr ); n_g.w.l = 0;
	n_b.w.h = BGR_B( m_packet.Sprite8x8.n_bgr ); n_b.w.l = 0;

	n_v = TEXTURE_V( m_packet.Sprite8x8.n_texture );
	n_y = COORD_Y( m_packet.Sprite8x8.n_coord ) + m_n_drawoffset_y;
	n_h = 8;

	while( n_h > 0 )
	{
		n_x = COORD_X( m_packet.Sprite8x8.n_coord ) + m_n_drawoffset_x;
		n_u = TEXTURE_U( m_packet.Sprite8x8.n_texture );

		n_distance = 8;
		if( n_distance > 0 && n_y >= (int32_t)m_n_drawarea_y1 && n_y <= (int32_t)m_n_drawarea_y2 )
		{
			if( ( (int32_t)m_n_drawarea_x1 - n_x ) > 0 )
			{
				n_u += ( m_n_drawarea_x1 - n_x );
				n_distance -= ( m_n_drawarea_x1 - n_x );
				n_x = m_n_drawarea_x1;
			}
			TEXTUREFILL( FLATTEXTUREDRECTANGLEUPDATE, n_u, n_v );
		}
		n_v++;
		n_y++;
		n_h--;
	}
}

static void Sprite16x16( void )
{
	int16_t n_y;
	int16_t n_x;

	uint8_t n_cmd;

	uint32_t n_clutx;
	uint32_t n_cluty;

	uint32_t n_tp;
	uint32_t n_tx;
	uint32_t n_ty;
	uint32_t n_abr;
	uint32_t n_ti;

	uint16_t *p_n_f;
	uint16_t *p_n_redb;
	uint16_t *p_n_greenb;
	uint16_t *p_n_blueb;
	uint16_t *p_n_redtrans;
	uint16_t *p_n_greentrans;
	uint16_t *p_n_bluetrans;

	PAIR n_r;
	PAIR n_g;
	PAIR n_b;
	uint8_t n_u;
	uint8_t n_v;

	int16_t n_distance;
	uint32_t n_h;
	uint16_t *p_vram;
	uint16_t *p_clut;
	uint16_t n_bgr;

#if defined( MAME_DEBUG )
	if( m_n_debugskip == 11 )
	{
		return;
	}
	DebugMesh( COORD_X( m_packet.Sprite16x16.n_coord ) + m_n_drawoffset_x, COORD_Y( m_packet.Sprite16x16.n_coord ) + m_n_drawoffset_y );
	DebugMesh( COORD_X( m_packet.Sprite16x16.n_coord ) + m_n_drawoffset_x + 7, COORD_Y( m_packet.Sprite16x16.n_coord ) + m_n_drawoffset_y );
	DebugMesh( COORD_X( m_packet.Sprite16x16.n_coord ) + m_n_drawoffset_x, COORD_Y( m_packet.Sprite16x16.n_coord ) + m_n_drawoffset_y + 7 );
	DebugMesh( COORD_X( m_packet.Sprite16x16.n_coord ) + m_n_drawoffset_x + 7, COORD_Y( m_packet.Sprite16x16.n_coord ) + m_n_drawoffset_y + 7 );
	DebugMeshEnd();
#endif

	n_cmd = BGR_C( m_packet.Sprite16x16.n_bgr );

	n_clutx = ( m_packet.Sprite16x16.n_texture.w.h & 0x3f ) << 4;
	n_cluty = ( m_packet.Sprite16x16.n_texture.w.h >> 6 ) & 0x3ff;

	TEXTURESETUP( m_n_drawmode )

	n_r.w.h = BGR_R( m_packet.Sprite16x16.n_bgr ); n_r.w.l = 0;
	n_g.w.h = BGR_G( m_packet.Sprite16x16.n_bgr ); n_g.w.l = 0;
	n_b.w.h = BGR_B( m_packet.Sprite16x16.n_bgr ); n_b.w.l = 0;

	n_v = TEXTURE_V( m_packet.Sprite16x16.n_texture );
	n_y = COORD_Y( m_packet.Sprite16x16.n_coord ) + m_n_drawoffset_y;
	n_h = 16;

	while( n_h > 0 )
	{
		n_x = COORD_X( m_packet.Sprite16x16.n_coord ) + m_n_drawoffset_x;
		n_u = TEXTURE_U( m_packet.Sprite16x16.n_texture );

		n_distance = 16;
		if( n_distance > 0 && n_y >= (int32_t)m_n_drawarea_y1 && n_y <= (int32_t)m_n_drawarea_y2 )
		{
			if( ( (int32_t)m_n_drawarea_x1 - n_x ) > 0 )
			{
				n_u += ( m_n_drawarea_x1 - n_x );
				n_distance -= ( m_n_drawarea_x1 - n_x );
				n_x = m_n_drawarea_x1;
			}
			TEXTUREFILL( FLATTEXTUREDRECTANGLEUPDATE, n_u, n_v );
		}
		n_v++;
		n_y++;
		n_h--;
	}
}

static void Dot( void )
{
	int32_t n_x;
	int32_t n_y;
	uint32_t n_r;
	uint32_t n_g;
	uint32_t n_b;
	uint16_t *p_vram;

#if defined( MAME_DEBUG )
	if( m_n_debugskip == 12 )
	{
		return;
	}
	DebugMesh( COORD_X( m_packet.Dot.vertex.n_coord ) + m_n_drawoffset_x, COORD_Y( m_packet.Dot.vertex.n_coord ) + m_n_drawoffset_y );
	DebugMeshEnd();
#endif

	n_r = BGR_R( m_packet.Dot.n_bgr );
	n_g = BGR_G( m_packet.Dot.n_bgr );
	n_b = BGR_B( m_packet.Dot.n_bgr );
	n_x = COORD_X( m_packet.Dot.vertex.n_coord ) + m_n_drawoffset_x;
	n_y = COORD_Y( m_packet.Dot.vertex.n_coord ) + m_n_drawoffset_y;

	if( (int16_t)n_x >= (int32_t)m_n_drawarea_x1 &&
		(int16_t)n_y >= (int32_t)m_n_drawarea_y1 &&
		(int16_t)n_x <= (int32_t)m_n_drawarea_x2 &&
		(int16_t)n_y <= (int32_t)m_n_drawarea_y2 )
	{
		p_vram = m_p_p_vram[ n_y ] + n_x;
		*( p_vram ) = m_p_n_redshade[ MID_LEVEL | n_r ] |
			m_p_n_greenshade[ MID_LEVEL | n_g ] |
			m_p_n_blueshade[ MID_LEVEL | n_b ];
	}
}

static void MoveImage( void )
{
	int16_t n_w;
	int16_t n_h;
	int16_t n_srcx;
	int16_t n_srcy;
	int16_t n_dsty;
	int16_t n_dstx;

#if defined( MAME_DEBUG )
	if( m_n_debugskip == 13 )
	{
		return;
	}
	DebugMesh( COORD_X( m_packet.MoveImage.vertex[ 1 ].n_coord ), COORD_Y( m_packet.MoveImage.vertex[ 1 ].n_coord ) );
	DebugMesh( COORD_X( m_packet.MoveImage.vertex[ 1 ].n_coord ) + SIZE_W( m_packet.MoveImage.n_size ), COORD_Y( m_packet.MoveImage.vertex[ 1 ].n_coord ) );
	DebugMesh( COORD_X( m_packet.MoveImage.vertex[ 1 ].n_coord ), COORD_Y( m_packet.MoveImage.vertex[ 1 ].n_coord ) + SIZE_H( m_packet.MoveImage.n_size ) );
	DebugMesh( COORD_X( m_packet.MoveImage.vertex[ 1 ].n_coord ) + SIZE_W( m_packet.MoveImage.n_size ), COORD_Y( m_packet.MoveImage.vertex[ 1 ].n_coord ) + SIZE_H( m_packet.MoveImage.n_size ) );
	DebugMeshEnd();
#endif

	n_srcy = COORD_Y( m_packet.MoveImage.vertex[ 0 ].n_coord );
	n_dsty = COORD_Y( m_packet.MoveImage.vertex[ 1 ].n_coord );
	n_h = SIZE_H( m_packet.MoveImage.n_size );

	while( n_h > 0 )
	{
		n_srcx = COORD_X( m_packet.MoveImage.vertex[ 0 ].n_coord );
		n_dstx = COORD_X( m_packet.MoveImage.vertex[ 1 ].n_coord );
		n_w = SIZE_W( m_packet.MoveImage.n_size );
		while( n_w > 0 )
		{
			*( m_p_p_vram[ n_dsty & 1023 ] + ( n_dstx & 1023 ) ) =
				*( m_p_p_vram[ n_srcy & 1023 ] + ( n_srcx & 1023 ) );
			n_srcx++;
			n_dstx++;
			n_w--;
		}
		n_srcy++;
		n_dsty++;
		n_h--;
	}
}

void psx_gpu_write( uint32_t *p_ram, int32_t n_size )
{
	while( n_size > 0 )
	{
		uint32_t data = *( p_ram );

		verboselog( 2, "PSX Packet #%u %08x\n", m_n_gpu_buffer_offset, data );
		m_packet.n_entry[ m_n_gpu_buffer_offset ] = data;
		switch( m_packet.n_entry[ 0 ] >> 24 )
		{
		case 0x00:
		case 0x01:
			break;
		case 0x02:
			if( m_n_gpu_buffer_offset < 2 )
			{
				m_n_gpu_buffer_offset++;
			}
			else
			{
#if 0
				verboselog( 1, "%02x: frame buffer rectangle %u,%u %u,%u\n", m_packet.n_entry[ 0 ] >> 24,
					m_packet.n_entry[ 1 ] & 0xffff, m_packet.n_entry[ 1 ] >> 16, m_packet.n_entry[ 2 ] & 0xffff, m_packet.n_entry[ 2 ] >> 16 );
#endif
				FrameBufferRectangleDraw();
				m_n_gpu_buffer_offset = 0;
			}
			break;
		case 0x20:
		case 0x21:
		case 0x22:
		case 0x23:
			if( m_n_gpu_buffer_offset < 3 )
			{
				m_n_gpu_buffer_offset++;
			}
			else
			{
				//verboselog( 1, "%02x: monochrome 3 point polygon\n", m_packet.n_entry[ 0 ] >> 24 );
				FlatPolygon( 3 );
				m_n_gpu_buffer_offset = 0;
			}
			break;
		case 0x24:
		case 0x25:
		case 0x26:
		case 0x27:
			if( m_n_gpu_buffer_offset < 6 )
			{
				m_n_gpu_buffer_offset++;
			}
			else
			{
				//verboselog( 1, "%02x: textured 3 point polygon\n", m_packet.n_entry[ 0 ] >> 24 );
				FlatTexturedPolygon( 3 );
				m_n_gpu_buffer_offset = 0;
			}
			break;
		case 0x28:
		case 0x29:
		case 0x2a:
		case 0x2b:
			if( m_n_gpu_buffer_offset < 4 )
			{
				m_n_gpu_buffer_offset++;
			}
			else
			{
				//verboselog( 1, "%02x: monochrome 4 point polygon\n", m_packet.n_entry[ 0 ] >> 24 );
				FlatPolygon( 4 );
				m_n_gpu_buffer_offset = 0;
			}
			break;
		case 0x2c:
		case 0x2d:
		case 0x2e:
		case 0x2f:
			if( m_n_gpu_buffer_offset < 8 )
			{
				m_n_gpu_buffer_offset++;
			}
			else
			{
				//verboselog( 1, "%02x: textured 4 point polygon\n", m_packet.n_entry[ 0 ] >> 24 );
				FlatTexturedPolygon( 4 );
				m_n_gpu_buffer_offset = 0;
			}
			break;
		case 0x30:
		case 0x31:
		case 0x32:
		case 0x33:
			if( m_n_gpu_buffer_offset < 5 )
			{
				m_n_gpu_buffer_offset++;
			}
			else
			{
				//verboselog( 1, "%02x: gouraud 3 point polygon\n", m_packet.n_entry[ 0 ] >> 24 );
				GouraudPolygon( 3 );
				m_n_gpu_buffer_offset = 0;
			}
			break;
		case 0x34:
		case 0x35:
		case 0x36:
		case 0x37:
			if( m_n_gpu_buffer_offset < 8 )
			{
				m_n_gpu_buffer_offset++;
			}
			else
			{
				//verboselog( 1, "%02x: gouraud textured 3 point polygon\n", m_packet.n_entry[ 0 ] >> 24 );
				GouraudTexturedPolygon( 3 );
				m_n_gpu_buffer_offset = 0;
			}
			break;
		case 0x38:
		case 0x39:
		case 0x3a:
		case 0x3b:
			if( m_n_gpu_buffer_offset < 7 )
			{
				m_n_gpu_buffer_offset++;
			}
			else
			{
				//verboselog( 1, "%02x: gouraud 4 point polygon\n", m_packet.n_entry[ 0 ] >> 24 );
				GouraudPolygon( 4 );
				m_n_gpu_buffer_offset = 0;
			}
			break;
		case 0x3c:
		case 0x3d:
		case 0x3e:
		case 0x3f:
			if( m_n_gpu_buffer_offset < 11 )
			{
				m_n_gpu_buffer_offset++;
			}
			else
			{
				//verboselog( 1, "%02x: gouraud textured 4 point polygon\n", m_packet.n_entry[ 0 ] >> 24 );
				GouraudTexturedPolygon( 4 );
				m_n_gpu_buffer_offset = 0;
			}
			break;
		case 0x40:
		case 0x41:
			if( m_n_gpu_buffer_offset < 2 )
			{
				m_n_gpu_buffer_offset++;
			}
			else
			{
				//verboselog( 1, "%02x: monochrome line\n", m_packet.n_entry[ 0 ] >> 24 );
				MonochromeLine();
				m_n_gpu_buffer_offset = 0;
			}
			break;
		case 0x48:
		case 0x4c:
			if( m_n_gpu_buffer_offset < 3 )
			{
				m_n_gpu_buffer_offset++;
			}
			else
			{
				//verboselog( 1, "%02x: monochrome polyline\n", m_packet.n_entry[ 0 ] >> 24 );
				MonochromeLine();
				if( ( m_packet.n_entry[ 3 ] & 0xf000f000 ) != 0x50005000 )
				{
					m_packet.n_entry[ 1 ] = m_packet.n_entry[ 2 ];
					m_packet.n_entry[ 2 ] = m_packet.n_entry[ 3 ];
					m_n_gpu_buffer_offset = 3;
				}
				else
				{
					m_n_gpu_buffer_offset = 0;
				}
			}
			break;
		case 0x50:
		case 0x52:
			if( m_n_gpu_buffer_offset < 3 )
			{
				m_n_gpu_buffer_offset++;
			}
			else
			{
				//verboselog( 1, "%02x: gouraud line\n", m_packet.n_entry[ 0 ] >> 24 );
				GouraudLine();
				m_n_gpu_buffer_offset = 0;
			}
			break;
		case 0x58:
		case 0x5c:
			if( m_n_gpu_buffer_offset < 5 &&
				( m_n_gpu_buffer_offset != 4 || ( m_packet.n_entry[ 4 ] & 0xf000f000 ) != 0x50005000 ) )
			{
				m_n_gpu_buffer_offset++;
			}
			else
			{
				//verboselog( 1, "%02x: gouraud polyline\n", m_packet.n_entry[ 0 ] >> 24 );
				GouraudLine();
				if( ( m_packet.n_entry[ 4 ] & 0xf000f000 ) != 0x50005000 )
				{
					m_packet.n_entry[ 0 ] = ( m_packet.n_entry[ 0 ] & 0xff000000 ) | ( m_packet.n_entry[ 2 ] & 0x00ffffff );
					m_packet.n_entry[ 1 ] = m_packet.n_entry[ 3 ];
					m_packet.n_entry[ 2 ] = m_packet.n_entry[ 4 ];
					m_packet.n_entry[ 3 ] = m_packet.n_entry[ 5 ];
					m_n_gpu_buffer_offset = 4;
				}
				else
				{
					m_n_gpu_buffer_offset = 0;
				}
			}
			break;
		case 0x60:
		case 0x61:
		case 0x62:
		case 0x63:
			if( m_n_gpu_buffer_offset < 2 )
			{
				m_n_gpu_buffer_offset++;
			}
			else
			{
#if 0
				verboselog( 1, "%02x: rectangle %d,%d %d,%d\n",
					m_packet.n_entry[ 0 ] >> 24,
					(int16_t)( m_packet.n_entry[ 1 ] & 0xffff ), (int16_t)( m_packet.n_entry[ 1 ] >> 16 ),
					(int16_t)( m_packet.n_entry[ 2 ] & 0xffff ), (int16_t)( m_packet.n_entry[ 2 ] >> 16 ) );
#endif
				FlatRectangle();
				m_n_gpu_buffer_offset = 0;
			}
			break;
		case 0x64:
		case 0x65:
		case 0x66:
		case 0x67:
			if( m_n_gpu_buffer_offset < 3 )
			{
				m_n_gpu_buffer_offset++;
			}
			else
			{
#if 0
				verboselog( 1, "%02x: sprite %d,%d %u,%u %08x, %08x\n",
					m_packet.n_entry[ 0 ] >> 24,
					(int16_t)( m_packet.n_entry[ 1 ] & 0xffff ), (int16_t)( m_packet.n_entry[ 1 ] >> 16 ),
					m_packet.n_entry[ 3 ] & 0xffff, m_packet.n_entry[ 3 ] >> 16,
					m_packet.n_entry[ 0 ], m_packet.n_entry[ 2 ] );
#endif
				FlatTexturedRectangle();
				m_n_gpu_buffer_offset = 0;
			}
			break;
		case 0x68:
			if( m_n_gpu_buffer_offset < 1 )
			{
				m_n_gpu_buffer_offset++;
			}
			else
			{
#if 0
				verboselog( 1, "%02x: dot %d,%d %08x\n",
					m_packet.n_entry[ 0 ] >> 24,
					(int16_t)( m_packet.n_entry[ 1 ] & 0xffff ), (int16_t)( m_packet.n_entry[ 1 ] >> 16 ),
					m_packet.n_entry[ 0 ] & 0xffffff );
#endif
				Dot();
				m_n_gpu_buffer_offset = 0;
			}
			break;
		case 0x74:
		case 0x77:
			if( m_n_gpu_buffer_offset < 2 )
			{
				m_n_gpu_buffer_offset++;
			}
			else
			{
#if 0
				verboselog( 1, "%02x: 8x8 sprite %08x %08x %08x\n", m_packet.n_entry[ 0 ] >> 24,
					m_packet.n_entry[ 0 ], m_packet.n_entry[ 1 ], m_packet.n_entry[ 2 ] );
#endif
				Sprite8x8();
				m_n_gpu_buffer_offset = 0;
			}
			break;
		case 0x7c:
		case 0x7f:
			if( m_n_gpu_buffer_offset < 2 )
			{
				m_n_gpu_buffer_offset++;
			}
			else
			{
#if 0
				verboselog( 1, "%02x: 16x16 sprite %08x %08x %08x\n", m_packet.n_entry[ 0 ] >> 24,
					m_packet.n_entry[ 0 ], m_packet.n_entry[ 1 ], m_packet.n_entry[ 2 ] );
#endif
				Sprite16x16();
				m_n_gpu_buffer_offset = 0;
			}
			break;
		case 0x80:
			if( m_n_gpu_buffer_offset < 3 )
			{
				m_n_gpu_buffer_offset++;
			}
			else
			{
#if 0
				verboselog( 1, "move image in frame buffer %08x %08x %08x %08x\n", m_packet.n_entry[ 0 ], m_packet.n_entry[ 1 ], m_packet.n_entry[ 2 ], m_packet.n_entry[ 3 ] );
#endif
				MoveImage();
				m_n_gpu_buffer_offset = 0;
			}
			break;
		case 0xa0:
			if( m_n_gpu_buffer_offset < 3 )
			{
				m_n_gpu_buffer_offset++;
			}
			else
			{
				uint32_t n_pixel;
				for( n_pixel = 0; n_pixel < 2; n_pixel++ )
				{
#if 0
					verboselog( 2, "send image to framebuffer ( pixel %u,%u = %u )\n",
						( m_n_vramx + m_packet.n_entry[ 1 ] ) & 1023,
						( m_n_vramy + ( m_packet.n_entry[ 1 ] >> 16 ) ) & 1023,
						data & 0xffff );
#endif

					*( m_p_p_vram[ ( m_n_vramy + ( m_packet.n_entry[ 1 ] >> 16 ) ) & 1023 ] + ( ( m_n_vramx + m_packet.n_entry[ 1 ] ) & 1023 ) ) = data & 0xffff;
					m_n_vramx++;
					if( m_n_vramx >= ( m_packet.n_entry[ 2 ] & 0xffff ) )
					{
						m_n_vramx = 0;
						m_n_vramy++;
						if( m_n_vramy >= ( m_packet.n_entry[ 2 ] >> 16 ) )
						{
#if 0
							verboselog( 1, "%02x: send image to framebuffer %u,%u %u,%u\n", m_packet.n_entry[ 0 ] >> 24,
								m_packet.n_entry[ 1 ] & 0xffff, ( m_packet.n_entry[ 1 ] >> 16 ),
								m_packet.n_entry[ 2 ] & 0xffff, ( m_packet.n_entry[ 2 ] >> 16 ) );
#endif
							m_n_gpu_buffer_offset = 0;
							m_n_vramx = 0;
							m_n_vramy = 0;
							break;
						}
					}
					data >>= 16;
				}
			}
			break;
		case 0xc0:
			if( m_n_gpu_buffer_offset < 2 )
			{
				m_n_gpu_buffer_offset++;
			}
			else
			{
#if 0
				verboselog( 1, "%02x: copy image from frame buffer\n", m_packet.n_entry[ 0 ] >> 24 );
#endif
				m_n_gpustatus |= ( 1L << 0x1b );
			}
			break;
		case 0xe1:
#if 0
			verboselog( 1, "%02x: draw mode %06x\n", m_packet.n_entry[ 0 ] >> 24,
				m_packet.n_entry[ 0 ] & 0xffffff );
#endif
			m_n_drawmode = m_packet.n_entry[ 0 ] & 0xffffff;
			if( m_n_gputype == 2 )
			{
				m_n_gpustatus = ( m_n_gpustatus & 0xfffff800 ) | ( m_n_drawmode & 0x7ff );
			}
			else
			{
				m_n_gpustatus = ( m_n_gpustatus & 0xffffe000 ) | ( m_n_drawmode & 0x1fff );
			}
			break;
		case 0xe2:
			m_n_twy = ( ( ( m_packet.n_entry[ 0 ] >> 15 ) & 0x1f ) << 3 );
			m_n_twx = ( ( ( m_packet.n_entry[ 0 ] >> 10 ) & 0x1f ) << 3 );
			m_n_twh = 256 - ( ( ( m_packet.n_entry[ 0 ] >> 5 ) & 0x1f ) << 3 );
			m_n_tww = 256 - ( ( m_packet.n_entry[ 0 ] & 0x1f ) << 3 );
#if 0
			verboselog( 1, "%02x: texture window %u,%u %u,%u\n", m_packet.n_entry[ 0 ] >> 24,
				m_n_twx, m_n_twy, m_n_tww, m_n_twh );
#endif
			break;
		case 0xe3:
			m_n_drawarea_x1 = m_packet.n_entry[ 0 ] & 1023;
			if( m_n_gputype == 2 )
			{
				m_n_drawarea_y1 = ( m_packet.n_entry[ 0 ] >> 10 ) & 1023;
			}
			else
			{
				m_n_drawarea_y1 = ( m_packet.n_entry[ 0 ] >> 12 ) & 1023;
			}
#if 0
			verboselog( 1, "%02x: drawing area top left %d,%d\n", m_packet.n_entry[ 0 ] >> 24,
				m_n_drawarea_x1, m_n_drawarea_y1 );
#endif
			break;
		case 0xe4:
			m_n_drawarea_x2 = m_packet.n_entry[ 0 ] & 1023;
			if( m_n_gputype == 2 )
			{
				m_n_drawarea_y2 = ( m_packet.n_entry[ 0 ] >> 10 ) & 1023;
			}
			else
			{
				m_n_drawarea_y2 = ( m_packet.n_entry[ 0 ] >> 12 ) & 1023;
			}
#if 0
			verboselog( 1, "%02x: drawing area bottom right %d,%d\n", m_packet.n_entry[ 0 ] >> 24,
				m_n_drawarea_x2, m_n_drawarea_y2 );
#endif
			break;
		case 0xe5:
			m_n_drawoffset_x = SINT11( m_packet.n_entry[ 0 ] & 2047 );
			if( m_n_gputype == 2 )
			{
				m_n_drawoffset_y = SINT11( ( m_packet.n_entry[ 0 ] >> 11 ) & 2047 );
			}
			else
			{
				m_n_drawoffset_y = SINT11( ( m_packet.n_entry[ 0 ] >> 12 ) & 2047 );
			}
#if 0
			verboselog( 1, "%02x: drawing offset %d,%d\n", m_packet.n_entry[ 0 ] >> 24,
				m_n_drawoffset_x, m_n_drawoffset_y );
#endif
			break;
		case 0xe6:
#if 0
			if( ( m_packet.n_entry[ 0 ] & 3 ) != 0 )
			{
				verboselog( 1, "not handled: mask setting %d\n", m_packet.n_entry[ 0 ] & 3 );
			}
			else
			{
				verboselog( 1, "mask setting %d\n", m_packet.n_entry[ 0 ] & 3 );
			}
#endif
			break;
		default:
#if defined( MAME_DEBUG )
			usrintf_showmessage_secs( 1, "unknown GPU packet %08x", m_packet.n_entry[ 0 ] );
#endif
#if 0
			verboselog( 0, "unknown GPU packet %08x (%08x)\n", m_packet.n_entry[ 0 ], data );
#endif
#if ( STOP_ON_ERROR )
			m_n_gpu_buffer_offset = 1;
#endif
			break;
		}
		p_ram++;
		n_size--;
	}
}

WRITE32_HANDLER( psx_gpu_w )
{
	switch( offset )
	{
	case 0x00:
		psx_gpu_write( &data, 1 );
		break;
	case 0x01:
		switch( data >> 24 )
		{
		case 0x00:
			verboselog( 1, "reset gpu\n" );
			m_n_gpu_buffer_offset = 0;
			m_n_gpustatus = 0x14802000;
			m_n_drawmode = 0;
			m_n_drawarea_x1 = 0;
			m_n_drawarea_y1 = 0;
			m_n_drawarea_x2 = 1023;
			m_n_drawarea_y2 = 1023;
			m_n_drawoffset_x = 0;
			m_n_drawoffset_y = 0;
			m_n_displaystartx = 0;
			m_n_displaystarty = 0;
			m_n_screenwidth = 256;
			m_n_screenheight = 240;
			m_n_vramx = 0;
			m_n_vramy = 0;
			m_n_twx = 0;
			m_n_twy = 0;
			m_n_twh = 255;
			m_n_tww = 255;
			break;
		case 0x01:
			verboselog( 1, "not handled: reset command buffer\n" );
			m_n_gpu_buffer_offset = 0;
			break;
		case 0x02:
			verboselog( 1, "not handled: reset irq\n" );
			break;
		case 0x03:
			m_n_gpustatus &= ~( 1L << 0x17 );
			m_n_gpustatus |= ( data & 0x01 ) << 0x17;
			break;
		case 0x04:
			verboselog( 1, "dma setup %d\n", data & 3 );
			m_n_gpustatus &= ~( 3L << 0x1d );
			m_n_gpustatus |= ( data & 0x03 ) << 0x1d;
			break;
		case 0x05:
			m_n_displaystartx = data & 1023;
			if( m_n_gputype == 2 )
			{
				m_n_displaystarty = ( data >> 10 ) & 1023;
			}
			else
			{
				m_n_displaystarty = ( data >> 12 ) & 1023;
			}
			//verboselog( 1, "start of display area %d %d\n", m_n_displaystartx, m_n_displaystarty );
			break;
		case 0x06:
			m_n_horiz_disstart = data & 4095;
			m_n_horiz_disend = ( data >> 12 ) & 4095;
			//verboselog( 1, "horizontal display range %d %d\n", m_n_horiz_disstart, m_n_horiz_disend );
			break;
		case 0x07:
			m_n_vert_disstart = data & 1023;
			m_n_vert_disend = ( data >> 10 ) & 2047;
			//verboselog( 1, "vertical display range %d %d\n", m_n_vert_disstart, m_n_vert_disend );
			break;
		case 0x08:
			//verboselog( 1, "display mode %02x\n", data & 0xff );
			m_n_gpustatus &= ~( 127L << 0x10 );
			m_n_gpustatus |= ( data & 0x3f ) << 0x11; /* width 0 + height + videmode + isrgb24 + isinter */
			m_n_gpustatus |= ( ( data & 0x40 ) >> 0x06 ) << 0x10; /* width 1 */
			m_b_reverseflag = ( data >> 7 ) & 1;
			switch( ( m_n_gpustatus >> 0x13 ) & 1 )
			{
			case 0:
				m_n_screenheight = 240;
				break;
			case 1:
				m_n_screenheight = 480;
				break;
			}
			switch( ( m_n_gpustatus >> 0x11 ) & 3 )
			{
			case 0:
				switch( ( m_n_gpustatus >> 0x10 ) & 1 )
				{
				case 0:
					m_n_screenwidth = 256;
					break;
				case 1:
					m_n_screenwidth = 384;
					break;
				}
				break;
			case 1:
				m_n_screenwidth = 320;
				break;
			case 2:
				m_n_screenwidth = 512;
				break;
			case 3:
				m_n_screenwidth = 640;
				break;
			}
			break;
		case 0x09:
			//verboselog( 1, "not handled: GPU Control 0x09: %08x\n", data );
			break;
		case 0x10:
			switch( data & 7 )
			{
			case 0x03:
				verboselog( 1, "GPU Info - Draw area top left\n" );
				if( m_n_gputype == 2 )
				{
					m_n_gpuinfo = m_n_drawarea_x1 | ( m_n_drawarea_y1 << 10 );
				}
				else
				{
					m_n_gpuinfo = m_n_drawarea_x1 | ( m_n_drawarea_y1 << 12 );
				}
				break;
			case 0x04:
				verboselog( 1, "GPU Info - Draw area bottom right\n" );
				if( m_n_gputype == 2 )
				{
					m_n_gpuinfo = m_n_drawarea_x2 | ( m_n_drawarea_y2 << 10 );
				}
				else
				{
					m_n_gpuinfo = m_n_drawarea_x2 | ( m_n_drawarea_y2 << 12 );
				}
				break;
			case 0x05:
				verboselog( 1, "GPU Info - Draw offset\n" );
				if( m_n_gputype == 2 )
				{
					m_n_gpuinfo = ( m_n_drawoffset_x & 2047 ) | ( ( m_n_drawoffset_y & 2047 ) << 11 );
				}
				else
				{
					m_n_gpuinfo = ( m_n_drawoffset_x & 2047 ) | ( ( m_n_drawoffset_y & 2047 ) << 12 );
				}
				break;
			case 0x07:
				verboselog( 1, "GPU Info - GPU Type\n" );
				m_n_gpuinfo = m_n_gputype;
				break;
			default:
				verboselog( 0, "GPU Info - unknown request (%08x)\n", data );
				m_n_gpuinfo = 0;
				break;
			}
			break;
		case 0x20:
			verboselog( 1, "not handled: GPU Control 0x20: %08x\n", data );
			break;
		default:
#if defined( MAME_DEBUG )
			usrintf_showmessage_secs( 1, "unknown GPU command %08x", data );
#endif
			verboselog( 0, "gpu_w( %08x ) unknown GPU command\n", data );
			break;
		}
		break;
	default:
		verboselog( 0, "gpu_w( %08x, %08x, %08x ) unknown register\n", offset, data, mem_mask );
		break;
	}
}


void psx_gpu_read( uint32_t *p_ram, int32_t n_size )
{
	while( n_size > 0 )
	{
		if( ( m_n_gpustatus & ( 1L << 0x1b ) ) != 0 )
		{
			uint32_t n_pixel;
			PAIR data;

			//verboselog( 2, "copy image from frame buffer ( %d, %d )\n", m_n_vramx, m_n_vramy );
			data.d = 0;
			for( n_pixel = 0; n_pixel < 2; n_pixel++ )
			{
				data.w.l = data.w.h;
				data.w.h = *( m_p_p_vram[ m_n_vramy + ( m_packet.n_entry[ 1 ] >> 16 ) ] + m_n_vramx + ( m_packet.n_entry[ 1 ] & 0xffff ) );
				m_n_vramx++;
				if( m_n_vramx >= ( m_packet.n_entry[ 2 ] & 0xffff ) )
				{
					m_n_vramx = 0;
					m_n_vramy++;
					if( m_n_vramy >= ( m_packet.n_entry[ 2 ] >> 16 ) )
					{
						//verboselog( 1, "copy image from frame buffer end\n" );
						m_n_gpustatus &= ~( 1L << 0x1b );
						m_n_gpu_buffer_offset = 0;
						m_n_vramx = 0;
						m_n_vramy = 0;
						if( n_pixel == 0 )
						{
							data.w.l = data.w.h;
							data.w.h = 0;
						}
						break;
					}
				}
			}
			*( p_ram ) = data.d;
		}
		else
		{
			//verboselog( 2, "read GPU info (%08x)\n", m_n_gpuinfo );
			*( p_ram ) = m_n_gpuinfo;
		}
		p_ram++;
		n_size--;
	}
}

READ32_HANDLER( psx_gpu_r )
{
	uint32_t data;

	switch( offset )
	{
	case 0x00:
		psx_gpu_read( &data, 1 );
		break;
	case 0x01:
		//verboselog( 1, "read GPU status (%08x)\n", m_n_gpustatus );
		data = m_n_gpustatus;
		break;
	default:
		//verboselog( 0, "gpu_r( %08x, %08x ) unknown register\n", offset, mem_mask );
		data = 0;
		break;
	}
	return data;
}

INTERRUPT_GEN( psx_vblank )
{
	m_n_gpustatus ^= ( 1L << 31 );
	psx_irq_set( 0x0001 );
}

void psx_gpu_reset( void )
{
	psx_gpu_w( 1, 0, 0 );
}
