#include "libretro.h"
#include "libretro_perf.h"
#include "mame2003.h"
#include "palette.h"
#include "fileio.h"
#include "common.h"
#include "mame.h"
#include "usrintrf.h"
#include "driver.h"

#define MAX_LED 16

extern retro_log_printf_t log_cb;
extern retro_environment_t environ_cb;
extern retro_video_refresh_t video_cb;
extern retro_set_led_state_t led_state_cb;

/* Part of libretro's API */
extern int gotFrame;

extern struct RunningMachine *Machine;

/* Native output bitmap conversion/transformation related vars */
struct osd_create_params video_config;
void (*video_pix_convert)(void *from, void *to);
unsigned video_stride_in, video_stride_out;
bool video_flip_x, video_flip_y, video_swap_xy;
bool video_hw_rotate;
const rgb_t *video_palette;
uint16_t *video_buffer;

/* Single pixel conversion functions */
static void pix_convert_pass8888(void *from, void *to);
static void pix_convert_1555to565(void *from, void *to);
static void pix_convert_passpal(void *from, void *to);
static void pix_convert_palto565(void *from, void *to);

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

/* Retrieve output geometry (i.e. window dimensions) */
void mame2003_video_get_geometry(unsigned *width, unsigned *height)
{
   *width = video_hw_rotate ? video_config.height : video_config.width;
   *height = video_hw_rotate ? video_config.width : video_config.height;
}

/* Init orientation and geometry */
void mame2003_video_init_orientation(void)
{
   unsigned orientation = Machine->gamedrv->flags & ORIENTATION_MASK;
   unsigned rotate_mode = 0;

   /*
      The UI is always oriented properly at start, but the externally applied
      orientation needs to be countered.
   */
   options.ui_orientation = reverse_orientation(orientation);

   /* TATE mode (90 degree CCW rotation), however, isn't countered */
   if (options.tate_mode)
   {
      if ((orientation & ROT180) == ORIENTATION_FLIP_X ||
	    (orientation & ROT180) == ORIENTATION_FLIP_Y)
         orientation ^= ROT180;
      orientation ^= ROT270;
   }

   /* Try to match orientation to a supported libretro rotation */
   rotate_mode = (orientation == ROT270) ? 1 : rotate_mode;
   rotate_mode = (orientation == ROT180) ? 2 : rotate_mode;
   rotate_mode = (orientation == ROT90) ? 3 : rotate_mode;

   video_hw_rotate = false;

   /* Try to use libretro to do a rotation */
   if (rotate_mode != 0
         && environ_cb(RETRO_ENVIRONMENT_SET_ROTATION, &rotate_mode))
   {
      video_hw_rotate = orientation & ORIENTATION_SWAP_XY;
      orientation = 0;
   }

   /* Set up native orientation flags that aren't handled by libretro */
   video_flip_x = orientation & ORIENTATION_FLIP_X;
   video_flip_y = orientation & ORIENTATION_FLIP_Y;
   video_swap_xy = orientation & ORIENTATION_SWAP_XY;

   Machine->ui_orientation = options.ui_orientation;

   /* Adjust for libretro's rotation not resizing output geometry */
   if (orientation & ORIENTATION_SWAP_XY)
   {
      int width = video_config.width;
      int height = video_config.height;
      
      video_config.width = height;
      video_config.height = width;
   }
}

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
         video_pix_convert = &pix_convert_passpal;
         color_mode = RETRO_PIXEL_FORMAT_XRGB8888;
      }
      /* Otherwise 16-bit RGB565 will suffice */
      else
      {
         video_stride_in = 2; video_stride_out = 2;
         video_pix_convert = &pix_convert_palto565;
         color_mode = RETRO_PIXEL_FORMAT_RGB565;
      }
   }

   /* Case II: 32-bit XRGB8888, pass it through */
   else if (video_config.depth == 32)
   {
      video_stride_in = 4; video_stride_out = 4;
      video_pix_convert = &pix_convert_pass8888;
      color_mode = RETRO_PIXEL_FORMAT_XRGB8888;

      /* TODO: figure those out */
      rgb_components[0] = 0x00FF0000;
      rgb_components[1] = 0x0000FF00;
      rgb_components[2] = 0x000000FF;
   }

   /* Case III: 16-bit 0RGB1555, convert it to RGB565 */
   else
   {
      video_stride_in = 2; video_stride_out = 2;
      video_pix_convert = &pix_convert_1555to565;
      color_mode = RETRO_PIXEL_FORMAT_RGB565;

      /* TODO: figure those out */
      rgb_components[0] = 0x00007C00;
      rgb_components[1] = 0x000003E0;
      rgb_components[2] = 0x0000001F;
   }

   environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &color_mode);
}

int osd_create_display(
   const struct osd_create_params *params, UINT32 *rgb_components)
{
   memcpy(&video_config, params, sizeof(video_config));    

   /* Determine video orientation and pixel format conversion settings */
   mame2003_video_init_orientation();
   mame2003_video_init_conversion(rgb_components);

   /* Allocate an output video buffer */
   video_buffer = malloc(video_config.width * video_config.height * video_stride_out);
   if (!video_buffer)
      return 1;

   return 0;
}

void osd_close_display(void)
{
   free(video_buffer);
   video_buffer = NULL;
}

static void pix_convert_pass8888(void *from, void *to)
{
   *((uint32_t*)to) = *((uint32_t*)from);
}

static void pix_convert_1555to565(void *from, void *to)
{
   const uint16_t color = *((uint16_t*)from);
   *((uint16_t*)to) = (color & 0xFFE0) << 1 | /* red, green */
                      (color & 0x001F) << 0;  /* blue */
}

static void pix_convert_passpal(void *from, void *to)
{
   const uint32_t color = video_palette[*((uint16_t*)from)];
   *((uint32_t*)to) = color;
}

static void pix_convert_palto565(void *from, void *to)
{
   const uint32_t color = video_palette[*((uint16_t*)from)];
   *((uint16_t*)to) = (color & 0x00F80000) >> 8 | /* red */
                      (color & 0x0000FC00) >> 5 | /* green */
                      (color & 0x000000F8) >> 3;  /* blue */
}

static void frame_convert(struct mame_display *display)
{
   int x, y;

   bool flip_x = video_flip_x;
   bool flip_y = video_flip_y;

   struct rectangle visible_area = display->game_visible_area;
   int x0 = visible_area.min_x;
   int y0 = visible_area.min_y;
   int x1 = visible_area.max_x;
   int y1 = visible_area.max_y;
 
   char *output = (char*)video_buffer;
   char **lines = (char**)display->game_bitmap->line;

   /* Now do the conversion, accounting for x/y swaps */
   if (!video_swap_xy)
   {
      /* Non-swapped */
      for (y = flip_y ? y1 : y0; flip_y ? y >= y0 : y <= y1; y += flip_y ? -1 : 1)
         for (x = flip_x ? x1 : x0; flip_x ? x >= x0 : x <= x1; x += flip_x ? -1 : 1)
         {
            video_pix_convert(
               &lines[y][x * video_stride_in], output);
            output += video_stride_out;
         }
   }
   else
   {
      /* Swapped */
      for (x = flip_y ? x1 : x0; flip_y ? x >= x0 : x <= x1; x += flip_y ? -1 : 1)
         for (y = flip_x ? y1 : y0; flip_x ? y >= y0 : y <= y1; y += flip_x ? -1 : 1)
         {
            video_pix_convert(
               &lines[y][x * video_stride_in], output);
            output += video_stride_out;
         }
   }
}


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

int osd_skip_this_frame(void)
{
   static unsigned frameskip_counter = 0;
   return frameskip_table[options.frameskip][frameskip_counter++ % 12];
}

void osd_update_video_and_audio(struct mame_display *display)
{
   RETRO_PERFORMANCE_INIT(perf_cb, update_video_and_audio);
   RETRO_PERFORMANCE_START(perf_cb, update_video_and_audio);

   if(display->changed_flags & 
      ( GAME_BITMAP_CHANGED | GAME_PALETTE_CHANGED
      | GAME_VISIBLE_AREA_CHANGED | VECTOR_PIXELS_CHANGED))
   {
      struct rectangle visible_area = display->game_visible_area;
      unsigned width = visible_area.max_x - visible_area.min_x + 1;
      unsigned height = visible_area.max_y - visible_area.min_y + 1;

      /* Account for native XY swap */
      if (video_swap_xy)
      {
         unsigned temp;
         temp = width; width = height; height = temp;
      }

      /* Update the UI area */
      if (display->changed_flags & GAME_VISIBLE_AREA_CHANGED)
         set_ui_visarea(
            visible_area.min_x, visible_area.min_y,
            visible_area.max_x, visible_area.max_y);

      /* Update the palette */
      if (display->changed_flags & GAME_PALETTE_CHANGED)
         video_palette = display->game_palette;

      /* Update the game bitmap */
      if (video_cb && !osd_skip_this_frame())
      {
         frame_convert(display);
         video_cb(video_buffer, width, height, width * video_stride_out);
      }
      else
         video_cb(NULL, width, height, width * video_stride_out);
   }

   /* Update LED indicators state */
   if (display->changed_flags & LED_STATE_CHANGED)
   {
       if(led_state_cb != NULL)
       {
           /* Set each changed LED */
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
               o >>= 1;
               n >>= 1;
           }
           prev_led_state = display->led_state;
       }
   }
   
   gotFrame = 1;

   RETRO_PERFORMANCE_STOP(perf_cb, update_video_and_audio);
}

struct mame_bitmap *osd_override_snapshot(struct mame_bitmap *bitmap, struct rectangle *bounds)
{
   return NULL;
}
