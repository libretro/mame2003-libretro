#include "libretro.h"
#include "libretro_perf.h"
#include "mame2003.h"
#include "palette.h"
#include "fileio.h"
#include "common.h"
#include "mame.h"
#include "usrintrf.h"
#include "driver.h"

extern retro_log_printf_t log_cb;
extern retro_environment_t environ_cb;
extern retro_video_refresh_t video_cb;
extern retro_set_led_state_t led_state_cb;
static unsigned long prev_led_state = 0;

#define MAX_LED 16

uint16_t *videoBuffer;
struct osd_create_params videoConfig;
extern unsigned retroOrientation;
extern unsigned retroColorMode;
int gotFrame;

/* Various conversion/transformation settings */
struct {
   const rgb_t *palette;
   bool flip_x, flip_y, swap_xy;
   ptrdiff_t stride_in, stride_out;
   void (*pix_convert)(void *from, void *to);
} videoConversion;

/* Single pixel conversion functions */
static void pix_convert_pass8888(void *from, void *to);
static void pix_convert_1555to565(void *from, void *to);
static void pix_convert_passpal(void *from, void *to);
static void pix_convert_palto565(void *from, void *to);


int osd_create_display(
   const struct osd_create_params *params, UINT32 *rgb_components)
{
   memcpy(&videoConfig, params, sizeof(videoConfig));    

   /* Case I: 16-bit indexed palette */
   if (videoConfig.depth == 16)
   {
      /* If a 6+ bits per color channel palette is used, do 32-bit XRGB8888 */
      if (videoConfig.video_attributes & VIDEO_NEEDS_6BITS_PER_GUN)
      {
         retroColorMode = RETRO_PIXEL_FORMAT_XRGB8888;

         videoConversion.stride_in = 2;
         videoConversion.stride_out = 4;
         videoConversion.pix_convert = &pix_convert_passpal;
      }
      /* Otherwise 16-bit 0RGB1555 will suffice */
      else
      {
         retroColorMode = RETRO_PIXEL_FORMAT_RGB565;

         videoConversion.stride_in = 2;
         videoConversion.stride_out = 2;
         videoConversion.pix_convert = &pix_convert_palto565;
      }
   }

   /* Case II: 32-bit XRGB8888, pass it through */
   else if (videoConfig.depth == 32)
   {
      retroColorMode = RETRO_PIXEL_FORMAT_XRGB8888;

      rgb_components[0] = 0x00FF0000;
      rgb_components[1] = 0x0000FF00;
      rgb_components[2] = 0x000000FF;

      videoConversion.stride_in = 4;
      videoConversion.stride_out = 4;
      videoConversion.pix_convert = &pix_convert_pass8888;
   }

   /* Case III: 16-bit 0RGB1555, convert it to RGB565 */
   else
   {
      /* Complain if got here by default */
      if (videoConfig.depth != 15) {
         if (log_cb)
            log_cb(RETRO_LOG_ERROR, "Invalid videoConfig.depth (%u)", videoConfig.depth);
         else
            fprintf(stderr, "Invalid videoConfig.depth (%u)\n", videoConfig.depth);
      }

      retroColorMode = RETRO_PIXEL_FORMAT_RGB565;

      rgb_components[0] = 0x00007C00;
      rgb_components[1] = 0x000003E0;
      rgb_components[2] = 0x0000001F;

      videoConversion.stride_in = 2;
      videoConversion.stride_out = 2;
      videoConversion.pix_convert = &pix_convert_1555to565;
   }

   /* Set up software rotation flags that aren't handled by libretro */
   videoConversion.flip_x = (bool)(retroOrientation & ORIENTATION_FLIP_X);
   videoConversion.flip_y = (bool)(retroOrientation & ORIENTATION_FLIP_Y);
   videoConversion.swap_xy = (bool)(retroOrientation & ORIENTATION_SWAP_XY);

   /* Allocate an output video buffer */
   videoBuffer = malloc(videoConfig.width * videoConfig.height * videoConversion.stride_out);
   if (!videoBuffer)
      return 1;

   environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &retroColorMode);

   return 0;
}

void osd_close_display(void)
{
   free(videoBuffer);
   videoBuffer = NULL;
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
   const uint32_t color = videoConversion.palette[*((uint16_t*)from)];
   *((uint32_t*)to) = color;
}

static void pix_convert_palto565(void *from, void *to)
{
   const uint32_t color = videoConversion.palette[*((uint16_t*)from)];
   *((uint16_t*)to) = (color & 0x00F80000) >> 8 | /* red */
                      (color & 0x0000FC00) >> 5 | /* green */
                      (color & 0x000000F8) >> 3;  /* blue */
}

static void frame_convert(struct mame_display *display)
{
   size_t x, y, swap_temp;

   char **lines = (char **) display->game_bitmap->line;

   /* Initial (default) orientation */
   const bool flip_x = videoConversion.flip_x;
   const bool flip_y = videoConversion.flip_y;
   size_t x0 = display->game_visible_area.min_x;
   size_t y0 = display->game_visible_area.min_y;
   size_t x1 = display->game_visible_area.max_x + 1;
   size_t y1 = display->game_visible_area.max_y + 1;
 
   /* Now do the conversion, accounting for x/y swap */
   if (!videoConversion.swap_xy)
   {
      /* Non-swapped */
      char *output = (char*)videoBuffer;
      for (y = flip_y ? y1 : y0; y != (flip_y ? y0 : y1); y += flip_y ? -1 : 1)
         for (x = flip_x ? x1 : x0; x != (flip_x ? x0 : x1); x += flip_x ? -1 : 1)
         {
            videoConversion.pix_convert(
               &lines[y][x * videoConversion.stride_in], output);
            output += videoConversion.stride_out;
         }
   }
   else
   {
      /* Swapped */
      char *output = (char *) videoBuffer;
      for (x = flip_y ? x1 : x0; x != (flip_y ? x0 : x1); x += flip_y ? -1 : 1)
         for (y = flip_x ? y1 : y0; y != (flip_x ? y0 : y1); y += flip_x ? -1 : 1)
         {
            videoConversion.pix_convert(
               &lines[y][x * videoConversion.stride_in], output);
            output += videoConversion.stride_out;
         }
   }
}


static const int frameskip_table[12][12] =
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
      size_t width, height;
      width = videoConfig.width;
      height = videoConfig.height;

      /* Update the UI area */
      if (display->changed_flags & GAME_VISIBLE_AREA_CHANGED)
      {
         set_ui_visarea(
               display->game_visible_area.min_x, display->game_visible_area.min_y,
               display->game_visible_area.max_x, display->game_visible_area.max_y);
      }

      /* Update the palette */
      videoConversion.palette = display->game_palette;

      /* Update the game bitmap */
      if (video_cb && display->changed_flags & GAME_BITMAP_CHANGED && (osd_skip_this_frame() == 0))
      {
         frame_convert(display);
         video_cb(videoBuffer, width, height, width * videoConversion.stride_out);
      }
      else
         video_cb(NULL, width, height, width * videoConversion.stride_out);
   }

   /* Update LED indicators state */
   if (display->changed_flags & LED_STATE_CHANGED)
   {
       if(led_state_cb != NULL)
       {
           // Set each changed LED
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
