#include "libretro.h"
#include "libretro_perf.h"
#include "retro_inline.h"
#include "mame2003.h"
#include "palette.h"
#include "fileio.h"
#include "common.h"
#include "mame.h"
#include "usrintrf.h"
#include "driver.h"

#define MAX_LED 8

extern retro_log_printf_t log_cb;
extern retro_environment_t environ_cb;
extern retro_video_refresh_t video_cb;
extern retro_set_led_state_t led_state_cb;

/* Part of libretro's API */
extern int gotFrame;

extern struct RunningMachine *Machine;

/* Output bitmap settings */
struct osd_create_params video_config;
unsigned vis_width, vis_height;
unsigned tate_mode;

/* Output bitmap native conversion/transformation related vars */
unsigned video_conversion_type;
unsigned video_do_bypass;
unsigned video_stride_in, video_stride_out;
bool video_flip_x, video_flip_y, video_swap_xy;
bool video_hw_transpose;
const rgb_t *video_palette;
/* Palette pre-resolved to the RGB565 output format, rebuilt only when the
   game palette changes, so the per-pixel path is a plain table lookup with no
   arithmetic. */
uint16_t *video_palette_565;
unsigned video_palette_565_entries;
uint16_t *video_buffer;

/* -1 = not yet probed, 0 = frontend has no software framebuffer (stop asking),
   1 = supported. Reset on each display (re)create. */
int sw_framebuffer_supported = -1;

/* Possible pixel conversions (see corresponding function far below) */
enum
{
   VCT_PASS8888,   /* 32bpp XRGB8888 -> XRGB8888, straight copy           */
   VCT_1555TO565,  /* 16bpp 0RGB1555 -> RGB565, fast bit shift            */
   VCT_PASSPAL,    /* 16bpp palette index -> XRGB8888, palette lookup     */
   VCT_PALTO565    /* 16bpp palette index -> RGB565, pre-resolved lookup  */
};

/* Retrieve output geometry (i.e. window dimensions) */
void mame2003_video_get_geometry(struct retro_game_geometry *geom)
{
   /* Shorter variable names, for readability */
   unsigned max_w = video_config.width;
   unsigned max_h = video_config.height;
   unsigned vis_w = vis_width > 0 ? vis_width : max_w;
   unsigned vis_h = vis_height > 0 ? vis_height : max_h;

   /* Maximum dimensions must accomodate all image orientations */
   unsigned max_dim = max_w > max_h ? max_w : max_h;
   geom->max_width = geom->max_height = max_dim;

   /* Hardware rotations don't resize the framebuffer, adjust for that */
   geom->base_width = video_hw_transpose ? vis_h : vis_w;
   geom->base_height = video_hw_transpose ? vis_w : vis_h;

   geom->aspect_ratio = video_hw_transpose ? (float)video_config.aspect_y / (float)video_config.aspect_x : (float)video_config.aspect_x / (float)video_config.aspect_y;

}

void mame2003_video_update_visible_area(struct mame_display *display)
{
   struct retro_game_geometry geom = { 0 };

   struct rectangle visible_area = display->game_visible_area;
   vis_width = visible_area.max_x - visible_area.min_x + 1;
   vis_height = visible_area.max_y - visible_area.min_y + 1;

   /* Adjust for native XY swap */
   if (video_swap_xy)
   {
      unsigned temp;
      temp = vis_width; vis_width = vis_height; vis_height = temp;
   }

   /* Update MAME's own UI visible area */
   set_ui_visarea(
      visible_area.min_x, visible_area.min_y,
      visible_area.max_x, visible_area.max_y);

   /* Notify libretro of the change */
   mame2003_video_get_geometry(&geom);
   environ_cb(RETRO_ENVIRONMENT_SET_GEOMETRY, &geom);
}

/* Compute a reverse of a given orientation, accounting for XY swaps */
static unsigned reverse_orientation(unsigned orientation)
{
   int result = orientation;
   if (orientation & ORIENTATION_SWAP_XY)
   {
      result = ORIENTATION_SWAP_XY;
      if (orientation & ORIENTATION_FLIP_X)
         result ^= ORIENTATION_FLIP_Y;
      if (orientation & ORIENTATION_FLIP_Y)
         result ^= ORIENTATION_FLIP_X;
   }
   return result;
}

/* Init video orientation and geometry */
void mame2003_video_init_orientation(void)
{
   unsigned orientation = Machine->gamedrv->flags & ORIENTATION_MASK;
   unsigned rotate_mode;

   rotate_mode = 0; /* Known invalid value */
   tate_mode = options.tate_mode;   /* Acknowledge that the TATE mode is handled */
   video_hw_transpose = false;

   /* test RA if rotation is working if not dont alter the orientation let mame handle it */
   options.ui_orientation = reverse_orientation(orientation);

   if (tate_mode && (orientation & ORIENTATION_SWAP_XY))
      orientation = reverse_orientation(orientation) ^ ROT270;


   if (orientation == ROT0 || orientation == ROT90 || orientation == ROT180 || orientation == ROT270)
   {
      if (environ_cb(RETRO_ENVIRONMENT_SET_ROTATION, &rotate_mode) )
      {
        log_cb(RETRO_LOG_INFO, LOGPRE "RetroArch will perform the rotation.\n");

        rotate_mode = (orientation == ROT270) ? 1
                    : (orientation == ROT180) ? 2
                    : (orientation == ROT90 ) ? 3
                    : rotate_mode;

        if (orientation & ORIENTATION_SWAP_XY) video_hw_transpose = true; /*do this before the rotation reverse*/
        orientation = reverse_orientation(orientation ^ orientation); /* undo mame rotation if retroarch can do it */
        environ_cb(RETRO_ENVIRONMENT_SET_ROTATION, &rotate_mode);
      }

      else
        log_cb(RETRO_LOG_INFO, LOGPRE "This port of RetroArch does not support rotation or it has been disabled. Mame will rotate internally.\n");

   }
   else
     log_cb(RETRO_LOG_INFO, LOGPRE "RetroArch does not support this type of rotation, using mame internal rotation instead.\n");

   tate_mode = options.tate_mode;

   /* Set up native orientation flags that aren't handled by libretro */
   if (orientation & ORIENTATION_SWAP_XY) video_hw_transpose = true; /*dont set this to false if RA changes the flags else vertical games swap the xy*/
   video_flip_x = orientation & ORIENTATION_FLIP_X;
   video_flip_y = orientation & ORIENTATION_FLIP_Y;
   video_swap_xy = orientation & ORIENTATION_SWAP_XY;
   log_cb(RETRO_LOG_DEBUG,"mame internal: video_flip_x:%u video_flip_y:%u video_swap_xy:%u video_hw_transpose:%u\n",video_flip_x,video_flip_y,video_swap_xy,video_hw_transpose);
   Machine->ui_orientation = options.ui_orientation;


}

/* Init video format conversion settings */
void mame2003_video_init_conversion(UINT32 *rgb_components)
{
   unsigned color_mode;

   /* Case I: 16-bit indexed palette */
   if (video_config.depth == 16)
   {
      /* If a 6+ bits per color channel palette is used, do 32-bit XRGB8888 */
      if (video_config.video_attributes & VIDEO_NEEDS_6BITS_PER_GUN)
      {
         video_stride_in = 2; video_stride_out = 4;
         video_conversion_type = VCT_PASSPAL;
         color_mode = RETRO_PIXEL_FORMAT_XRGB8888;
      }
      /* Otherwise 16-bit RGB565 will suffice */
      else
      {
         video_stride_in = 2; video_stride_out = 2;
         video_conversion_type = VCT_PALTO565;
         color_mode = RETRO_PIXEL_FORMAT_RGB565;
      }
   }

   /* Case II: 32-bit XRGB8888, pass it through */
   else if (video_config.depth == 32)
   {
      video_stride_in = 4; video_stride_out = 4;
      video_conversion_type = VCT_PASS8888;
      color_mode = RETRO_PIXEL_FORMAT_XRGB8888;

      /* TODO: figure those out */
      rgb_components[0] = 0x00FF0000;
      rgb_components[1] = 0x0000FF00;
      rgb_components[2] = 0x000000FF;
   }

   /* Case III: 16-bit 0RGB1555. RetroArch's 0RGB1555 path is not a fast path,
      so convert up to RGB565 (a single shift per pixel) and let the frontend
      use its optimized RGB565 pipeline. */
   else if (video_config.depth == 15)
   {
      video_stride_in = 2; video_stride_out = 2;
      video_conversion_type = VCT_1555TO565;
      color_mode = RETRO_PIXEL_FORMAT_RGB565;
   }

   /* Otherwise bail out on unknown video mode */
   else
   {
      if (log_cb)
         log_cb(RETRO_LOG_ERROR, "Unsupported color depth: %u\n", video_config.depth);
      abort();
   }

   environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &color_mode);
}

/* Do a soft reinit to process new video output parameters */
void mame2003_video_reinit(void)
{
   UINT32 rgb_components[3];
   struct osd_create_params old_params = video_config;
   osd_close_display();
   osd_create_display(&old_params, &rgb_components[0]);
}

int osd_create_display(
   const struct osd_create_params *params, UINT32 *rgb_components)
{
   memcpy(&video_config, params, sizeof(video_config));

   /* Re-probe software-framebuffer support (the video driver may have changed). */
   sw_framebuffer_supported = -1;

   mame2003_video_init_orientation();
   mame2003_video_init_conversion(rgb_components);

   /* Check if a framebuffer conversion can be bypassed */
   /* A pointer straight into the game bitmap can be handed to the frontend
      only when no transform is needed at all: that means 32bpp XRGB8888
      (already the output format) with no flip/swap. Everything else needs a
      one-pass conversion (and that pass writes directly into the frontend's
      framebuffer when available, see below). */
   video_do_bypass =
      !video_flip_x && !video_flip_y && !video_swap_xy &&
      (video_config.depth == 32);
   /* Allocate an output video buffer, if necessary */
   if (!video_do_bypass)
   {
      video_buffer = malloc(video_config.width * video_config.height * video_stride_out);
      if (!video_buffer)
         return 1;
   }

   return 0;
}

void osd_close_display(void)
{
   free(video_buffer);
   video_buffer = NULL;
   free(video_palette_565);
   video_palette_565 = NULL;
   video_palette_565_entries = 0;
}

/* Rebuild the RGB565 palette lookup from the game's adjusted palette. Called
   only when the palette actually changes, moving the XRGB8888->RGB565
   conversion off the per-pixel path and onto the (much rarer) palette update. */
static void rebuild_palette_565(const rgb_t *pal, unsigned entries)
{
   unsigned i;

   if (video_palette_565_entries != entries)
   {
      free(video_palette_565);
      video_palette_565 = (uint16_t*)malloc(entries * sizeof(uint16_t));
      video_palette_565_entries = video_palette_565 ? entries : 0;
   }
   if (!video_palette_565)
      return;

   for (i = 0; i < entries; i++)
   {
      const uint32_t c = pal[i];
      video_palette_565[i] = (uint16_t)(
         (c & 0x00F80000) >> 8 | /* red   */
         (c & 0x0000FC00) >> 5 | /* green */
         (c & 0x000000F8) >> 3); /* blue  */
   }
}

/* Convert (and optionally flip/rotate) the visible area of the game bitmap
   into out_base, whose scanline stride is out_pitch bytes. The per-pixel work
   is written inline so each loop is a simple, vectorizable kernel, and the
   common no-transform case collapses to a flat loop (or memcpy) when the
   source region and destination are both tightly packed. */
static void frame_convert(struct mame_display *display, void *out_base, unsigned out_pitch)
{
   const struct rectangle va = display->game_visible_area;
   const int x0 = va.min_x, y0 = va.min_y, x1 = va.max_x, y1 = va.max_y;
   const int w  = x1 - x0 + 1, h = y1 - y0 + 1;
   const int in_pitch = display->game_bitmap->rowpixels; /* source pixels/row */
   const int out_px   = (int)(out_pitch / (unsigned)video_stride_out); /* dest pixels/row */
   const int flip_x = video_flip_x, flip_y = video_flip_y;
   const void *base = display->game_bitmap->base;
   int x, y, i;

   /* XY swap: transposed output. Rare - only when the frontend cannot rotate. */
   if (video_swap_xy)
   {
      switch (video_conversion_type)
      {
         case VCT_PASS8888: {
            const uint32_t *in = (const uint32_t*)base; uint32_t *o = (uint32_t*)out_base;
            for (x = flip_y?x1:x0; flip_y?x>=x0:x<=x1; x += flip_y?-1:1) {
               for (y = flip_x?y1:y0; flip_x?y>=y0:y<=y1; y += flip_x?-1:1) *o++ = in[y*in_pitch + x];
               o += out_px - h; }
         } break;
         case VCT_1555TO565: {
            const uint16_t *in = (const uint16_t*)base; uint16_t *o = (uint16_t*)out_base;
            for (x = flip_y?x1:x0; flip_y?x>=x0:x<=x1; x += flip_y?-1:1) {
               for (y = flip_x?y1:y0; flip_x?y>=y0:y<=y1; y += flip_x?-1:1) { uint16_t p = in[y*in_pitch + x]; *o++ = (uint16_t)(((p&0x7FE0)<<1)|(p&0x1F)); }
               o += out_px - h; }
         } break;
         case VCT_PASSPAL: {
            const uint16_t *in = (const uint16_t*)base; uint32_t *o = (uint32_t*)out_base;
            for (x = flip_y?x1:x0; flip_y?x>=x0:x<=x1; x += flip_y?-1:1) {
               for (y = flip_x?y1:y0; flip_x?y>=y0:y<=y1; y += flip_x?-1:1) *o++ = video_palette[in[y*in_pitch + x]];
               o += out_px - h; }
         } break;
         case VCT_PALTO565: {
            const uint16_t *in = (const uint16_t*)base; uint16_t *o = (uint16_t*)out_base;
            for (x = flip_y?x1:x0; flip_y?x>=x0:x<=x1; x += flip_y?-1:1) {
               for (y = flip_x?y1:y0; flip_x?y>=y0:y<=y1; y += flip_x?-1:1) *o++ = video_palette_565[in[y*in_pitch + x]];
               o += out_px - h; }
         } break;
      }
      return;
   }

   /* No transform and both sides tightly packed: one flat loop (best
      vectorization), and a pure copy becomes a memcpy. */
   if (!flip_x && !flip_y && x0 == 0 && w == in_pitch && out_px == w)
   {
      const int n = w * h;
      switch (video_conversion_type)
      {
         case VCT_PASS8888: {
            const uint32_t *in = (const uint32_t*)base + (size_t)y0*in_pitch;
            memcpy(out_base, in, (size_t)n * sizeof(uint32_t));
         } break;
         case VCT_1555TO565: {
            const uint16_t *in = (const uint16_t*)base + (size_t)y0*in_pitch; uint16_t *o = (uint16_t*)out_base;
            for (i = 0; i < n; i++) { uint16_t p = in[i]; o[i] = (uint16_t)(((p&0x7FE0)<<1)|(p&0x1F)); }
         } break;
         case VCT_PASSPAL: {
            const uint16_t *in = (const uint16_t*)base + (size_t)y0*in_pitch; uint32_t *o = (uint32_t*)out_base;
            for (i = 0; i < n; i++) o[i] = video_palette[in[i]];
         } break;
         case VCT_PALTO565: {
            const uint16_t *in = (const uint16_t*)base + (size_t)y0*in_pitch; uint16_t *o = (uint16_t*)out_base;
            for (i = 0; i < n; i++) o[i] = video_palette_565[in[i]];
         } break;
      }
      return;
   }

   /* General no-swap path: one tight inner loop per row, flips fold into the
      row selection (flip_y) and column direction (flip_x). */
   switch (video_conversion_type)
   {
      case VCT_PASS8888:
         for (y = 0; y < h; y++) {
            const uint32_t *ir = (const uint32_t*)base + (size_t)(flip_y?(y1-y):(y0+y))*in_pitch;
            uint32_t *o = (uint32_t*)out_base + (size_t)y*out_px;
            if (!flip_x) for (x = 0; x < w; x++) o[x] = ir[x0+x];
            else         for (x = 0; x < w; x++) o[x] = ir[x1-x];
         }
         break;
      case VCT_1555TO565:
         for (y = 0; y < h; y++) {
            const uint16_t *ir = (const uint16_t*)base + (size_t)(flip_y?(y1-y):(y0+y))*in_pitch;
            uint16_t *o = (uint16_t*)out_base + (size_t)y*out_px;
            if (!flip_x) for (x = 0; x < w; x++) { uint16_t p = ir[x0+x]; o[x] = (uint16_t)(((p&0x7FE0)<<1)|(p&0x1F)); }
            else         for (x = 0; x < w; x++) { uint16_t p = ir[x1-x]; o[x] = (uint16_t)(((p&0x7FE0)<<1)|(p&0x1F)); }
         }
         break;
      case VCT_PASSPAL:
         for (y = 0; y < h; y++) {
            const uint16_t *ir = (const uint16_t*)base + (size_t)(flip_y?(y1-y):(y0+y))*in_pitch;
            uint32_t *o = (uint32_t*)out_base + (size_t)y*out_px;
            if (!flip_x) for (x = 0; x < w; x++) o[x] = video_palette[ir[x0+x]];
            else         for (x = 0; x < w; x++) o[x] = video_palette[ir[x1-x]];
         }
         break;
      case VCT_PALTO565:
         for (y = 0; y < h; y++) {
            const uint16_t *ir = (const uint16_t*)base + (size_t)(flip_y?(y1-y):(y0+y))*in_pitch;
            uint16_t *o = (uint16_t*)out_base + (size_t)y*out_px;
            if (!flip_x) for (x = 0; x < w; x++) o[x] = video_palette_565[ir[x0+x]];
            else         for (x = 0; x < w; x++) o[x] = video_palette_565[ir[x1-x]];
         }
         break;
   }
}

/* Try to obtain the frontend's framebuffer for this frame so the conversion
   can write straight into video memory (zero copy on the frontend side). We
   only ask for WRITE access. Returns true and fills *fb when a usable buffer
   in our exact output format is granted; false otherwise, in which case the
   caller falls back to the internal buffer. */
static bool get_current_sw_framebuffer(unsigned w, unsigned h, struct retro_framebuffer *fb)
{
   enum retro_pixel_format want =
      (video_stride_out == 4) ? RETRO_PIXEL_FORMAT_XRGB8888
                              : RETRO_PIXEL_FORMAT_RGB565;

   if (sw_framebuffer_supported == 0)
      return false;

   memset(fb, 0, sizeof(*fb));
   fb->width        = w;
   fb->height       = h;
   fb->access_flags = RETRO_MEMORY_ACCESS_WRITE;

   if (!environ_cb(RETRO_ENVIRONMENT_GET_CURRENT_SOFTWARE_FRAMEBUFFER, fb))
   {
      /* Unsupported by this frontend/video driver; don't probe every frame. */
      sw_framebuffer_supported = 0;
      return false;
   }
   sw_framebuffer_supported = 1;
   if (!fb->data)
      return false;
   /* The frontend may hand back a format different from the one we set; only
      use the buffer when it matches what we convert to. */
   if (fb->format != want)
      return false;

   return true;
}

extern bool retro_audio_buff_underrun;
extern bool retro_audio_buff_active;
extern unsigned retro_audio_buff_occupancy;

const int frameskip_table[12][12] =
   { { 0,0,0,0,0,0,0,0,0,0,0,0 },
     { 0,0,0,0,0,0,0,0,0,0,0,1 },
     { 0,0,0,0,0,1,0,0,0,0,0,1 },
     { 0,0,0,1,0,0,0,1,0,0,0,1 },
     { 0,0,1,0,0,1,0,0,1,0,0,1 },
     { 0,1,0,0,1,0,1,0,0,1,0,1 },
     { 0,1,0,1,0,1,0,1,0,1,0,1 },
     { 0,1,0,1,1,0,1,0,1,1,0,1 },
     { 0,1,1,0,1,1,0,1,1,0,1,1 },
     { 0,1,1,1,0,1,1,1,0,1,1,1 },
     { 0,1,1,1,1,1,0,1,1,1,1,1 },
     { 0,1,1,1,1,1,1,1,1,1,1,1 } };

UINT8 frameskip_counter = 0;

int osd_skip_this_frame(void)
{
	bool skip_frame = 0;

	if (pause_action)  return 0;  /* dont skip pause action hack (rendering mame info screens or you wont see them and not know to press a key) */

/*auto frame skip options */
	if(options.frameskip >0 && options.frameskip >= 12)
	{
		if ( retro_audio_buff_active)
		{
			switch ( options.frameskip)
			{
				case 12: /* auto */
					skip_frame = retro_audio_buff_underrun ? 1 : 0;
				break;
				case 13: /* aggressive */
					skip_frame = (retro_audio_buff_occupancy < 33)  ? 1 : 0;
				break;
				case 14: /* max */
					skip_frame = (retro_audio_buff_occupancy < 50)  ? 1 : 0;
				break;
				default:
					skip_frame = options.frameskip;
				break;
			}
		}
	}
	else /*manual frameskip */
	 skip_frame = frameskip_table[options.frameskip][frameskip_counter];

	return skip_frame;
}

void osd_update_video_and_audio(struct mame_display *display)
{
   RETRO_PERFORMANCE_INIT(perf_cb, update_video_and_audio);
   RETRO_PERFORMANCE_START(perf_cb, update_video_and_audio);

   if(display->changed_flags &
      ( GAME_BITMAP_CHANGED | GAME_PALETTE_CHANGED
      | GAME_VISIBLE_AREA_CHANGED | VECTOR_PIXELS_CHANGED))
   {
      /* Reinit video output on TATE mode toggle */
      if (options.tate_mode != tate_mode)
      {
         mame2003_video_reinit();
         display->changed_flags |= GAME_VISIBLE_AREA_CHANGED;
         tate_mode = options.tate_mode;
      }

      /* Update the visible area */
      if (display->changed_flags & GAME_VISIBLE_AREA_CHANGED)
         mame2003_video_update_visible_area(display);

      /* Update the palette */
      if (display->changed_flags & GAME_PALETTE_CHANGED)
      {
         video_palette = display->game_palette;
         if (video_conversion_type == VCT_PALTO565)
            rebuild_palette_565(display->game_palette, display->game_palette_entries);
      }

      /* Update the game bitmap */
      if (video_cb)
      {
         if (!osd_skip_this_frame())
         {
            if (video_do_bypass)
            {
               unsigned min_y = display->game_visible_area.min_y;
               unsigned min_x = display->game_visible_area.min_x;
               unsigned pitch = display->game_bitmap->rowpixels * video_stride_out;
               char *base = &((char*)display->game_bitmap->base)[min_y*pitch + min_x*video_stride_out];
               video_cb(base, vis_width, vis_height, pitch);
            }
            else
            {
               struct retro_framebuffer fb;
               void     *out       = video_buffer;
               unsigned  out_pitch = vis_width * video_stride_out;

               /* Write the converted frame straight into the frontend's
                  framebuffer when it grants one (no internal->frontend copy).
                  The XY-swap path always uses the internal buffer, since it
                  writes contiguously. Otherwise fall back to video_buffer. */
               if (!video_swap_xy &&
                   get_current_sw_framebuffer(vis_width, vis_height, &fb))
               {
                  out       = fb.data;
                  out_pitch = (unsigned)fb.pitch;
               }

               frame_convert(display, out, out_pitch);
               video_cb(out, vis_width, vis_height, out_pitch);
            }
         }
         else
            video_cb(NULL, vis_width, vis_height, vis_width * video_stride_out);
      }
   }
   else if (video_cb)
   {
      /* Nothing in the game display changed this frame. Still emit exactly
         one (duplicated) frame, so the frontend gets one video update per
         retro_run -- matching the one-frame-per-retro_run contract that
         run-ahead/recording rely on. vis_width/vis_height were set on an
         earlier GAME_VISIBLE_AREA_CHANGED and persist. */
      video_cb(NULL, vis_width, vis_height, vis_width * video_stride_out);
   }

   /* Update LED indicators state */
   if (led_state_cb && display->changed_flags & LED_STATE_CHANGED)
   {
      static unsigned long prev_led_state = 0;
      unsigned long o = prev_led_state;
      unsigned long n = display->led_state;
      int led;
      for(led=0;led<MAX_LED;led++)
      {
         if((o & 0x01) != (n & 0x01))
         {
            led_state_cb(led,n&0x01);
         }
         o >>= 1; n >>= 1;
      }
      prev_led_state = display->led_state;
   }

   gotFrame = 1;
  

   RETRO_PERFORMANCE_STOP(perf_cb, update_video_and_audio);
}

struct mame_bitmap *osd_override_snapshot(struct mame_bitmap *bitmap, struct rectangle *bounds)
{
   return NULL;
}
