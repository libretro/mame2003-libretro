/***************************************************************************

	poly.h

	Helper routines for polygon rendering.

***************************************************************************/

#include "driver.h"


/* up to 6 params (Z,U,V,R,G,B), plus it makes the vertex struct a nice size */
#define MAX_VERTEX_PARAMS		6
#define MAX_POLY_SCANLINES		512

struct poly_vertex
{
	int32_t	x;							/* 16.0 screen X coordinate */
	int32_t	y;							/* 16.0 screen Y coordinate */
	int32_t	p[MAX_VERTEX_PARAMS];		/* 32.0 interpolated parameter values */
};

struct poly_scanline
{
	int32_t	sx, ex;						/* 16.0 starting and ending X coordinates */
	int64_t	p[MAX_VERTEX_PARAMS];		/* 32.16 starting parameter values (at left) */
};

struct poly_scanline_data
{
	int32_t	sy, ey;						/* 16.0 starting and ending Y coordinates */
	int64_t	dp[MAX_VERTEX_PARAMS];		/* 32.16 per-pixel deltas for each parameter */
	struct poly_scanline scanline[MAX_POLY_SCANLINES];
};

const struct poly_scanline_data *setup_triangle_0(const struct poly_vertex *v1, const struct poly_vertex *v2, const struct poly_vertex *v3, const struct rectangle *cliprect);
const struct poly_scanline_data *setup_triangle_1(const struct poly_vertex *v1, const struct poly_vertex *v2, const struct poly_vertex *v3, const struct rectangle *cliprect);
const struct poly_scanline_data *setup_triangle_2(const struct poly_vertex *v1, const struct poly_vertex *v2, const struct poly_vertex *v3, const struct rectangle *cliprect);
const struct poly_scanline_data *setup_triangle_3(const struct poly_vertex *v1, const struct poly_vertex *v2, const struct poly_vertex *v3, const struct rectangle *cliprect);
const struct poly_scanline_data *setup_triangle_4(const struct poly_vertex *v1, const struct poly_vertex *v2, const struct poly_vertex *v3, const struct rectangle *cliprect);
const struct poly_scanline_data *setup_triangle_5(const struct poly_vertex *v1, const struct poly_vertex *v2, const struct poly_vertex *v3, const struct rectangle *cliprect);
const struct poly_scanline_data *setup_triangle_6(const struct poly_vertex *v1, const struct poly_vertex *v2, const struct poly_vertex *v3, const struct rectangle *cliprect);
