/*********************************************************************

	mame2003.c

    a port of Xmame 0.78 to the libretro API

*********************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <string/stdstring.h>
#include <libretro.h>
#include <file/file_path.h>
#include <streams/file_stream.h>
#include <math.h>
#include <string.h>

#if (HAS_DRZ80 || HAS_CYCLONE)
#include "frontend_list.h"
#endif

#include "mame.h"
#include "driver.h"
#include "state.h"
#include "log.h"
#include "input.h"
#include "inptport.h"
#include "fileio.h"
#include "controls.h"
#include "usrintrf.h"

int  pressure_check =  1.28 * 20;
int convert_analog_scale(int input);

int gotFrame;
const struct GameDriver  *game_driver;
static float              delta_samples;
int                       samples_per_frame = 0;
int                       orig_samples_per_frame =0;
short*                    samples_buffer;
short*                    conversion_buffer;
int                       usestereo = 1;
int16_t                   prev_pointer_x;
int16_t                   prev_pointer_y;
unsigned                  retroColorMode;
unsigned long             lastled = 0;

extern const struct KeyboardInfo retroKeys[];
extern int          retroKeyState[512];
int                 retroJsState[109]= {0}; // initialise to zero - we are only reading 4 players atm
extern int16_t      mouse_x[4];
extern int16_t      mouse_y[4];
extern int16_t      analogjoy[4][4];
struct ipd          *default_inputs; /* pointer the array of structs with default MAME input mappings and labels */

static struct retro_input_descriptor empty[] = { { 0 } };

retro_log_printf_t                 log_cb;
static struct retro_message        frontend_message;

struct                             retro_perf_callback perf_cb;
retro_environment_t                environ_cb                    = NULL;
retro_video_refresh_t              video_cb                      = NULL;
static retro_input_poll_t          poll_cb                       = NULL;
static retro_input_state_t         input_cb                      = NULL;
static retro_audio_sample_batch_t  audio_batch_cb                = NULL;
retro_set_led_state_t              led_state_cb                  = NULL;
struct retro_audio_buffer_status_callback buf_status_cb;


/******************************************************************************

  private function prototypes

******************************************************************************/

static void   check_system_specs(void);
       void   retro_describe_controls(void);
       int    get_mame_ctrl_id(int display_idx, int retro_ID);
static void   configure_cyclone_mode (int driverIndex);


/******************************************************************************

	frontend message interface

******************************************************************************/
void frontend_message_cb(const char *message_string, unsigned frames_to_display)
{
  frontend_message.msg    = message_string;
  frontend_message.frames = frames_to_display;
  environ_cb(RETRO_ENVIRONMENT_SET_MESSAGE, &frontend_message);
}

/******************************************************************************

  implementation of key libretro functions

******************************************************************************/
bool retro_audio_buff_active        = false;
unsigned retro_audio_buff_occupancy = 0;
bool retro_audio_buff_underrun      = false;

static void retro_audio_buff_status_cb(bool active, unsigned occupancy, bool underrun_likely)
{
   retro_audio_buff_active    = active;
   retro_audio_buff_occupancy = occupancy;
   retro_audio_buff_underrun  = underrun_likely;
}

void retro_set_audio_buff_status_cb(void)
{
  if (options.frameskip >0 && options.frameskip >= 12)
  {

      if (!environ_cb(RETRO_ENVIRONMENT_SET_AUDIO_BUFFER_STATUS_CALLBACK,
            &buf_status_cb))
      {
         if (log_cb)
            log_cb(RETRO_LOG_WARN, "Frameskip disabled - frontend does not support audio buffer status monitoring.\n");

         retro_audio_buff_active    = false;
         retro_audio_buff_occupancy = 0;
         retro_audio_buff_underrun  = false;
      }
      else
      log_cb(RETRO_LOG_INFO, "Frameskip Enabled\n");
  }
   else
      environ_cb(RETRO_ENVIRONMENT_SET_AUDIO_BUFFER_STATUS_CALLBACK,NULL);

}

void retro_init (void)
{
  struct retro_log_callback log;
  if (environ_cb(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &log))
    log_cb = log.log;
  else
    log_cb = NULL;

#ifdef LOG_PERFORMANCE
  environ_cb(RETRO_ENVIRONMENT_GET_PERF_INTERFACE, &perf_cb);
#endif

  check_system_specs();
}

static void check_system_specs(void)
{
  #ifndef RETRO_PROFILE
  #define RETRO_PROFILE 10
  #endif
   /* This value can be set in the Makefile, and then modified here before being passed,
    * even on an individual game basis.
    * However, as of June 2021, the libretro performance profile callback is not known
    * to be implemented by any frontends including RetroArch. The RA developers do not
    * have a suggested range of values. We use 10 by convention (copying other cores).
    */
   unsigned level = (unsigned)RETRO_PROFILE;
   environ_cb(RETRO_ENVIRONMENT_SET_PERFORMANCE_LEVEL, &level);
}

void retro_set_environment(retro_environment_t cb)
{
  struct retro_vfs_interface_info vfs_iface_info;

  environ_cb = cb;

  /* Initialise VFS */
  vfs_iface_info.required_interface_version = 1;
  vfs_iface_info.iface                      = NULL;
  if (environ_cb(RETRO_ENVIRONMENT_GET_VFS_INTERFACE, &vfs_iface_info))
    filestream_vfs_init(&vfs_iface_info);
}

void retro_get_system_av_info(struct retro_system_av_info *info)
{
  mame2003_video_get_geometry(&info->geometry);
  if(options.machine_timing)
  {
    if (Machine->drv->frames_per_second < 60.0 )
      info->timing.fps = 60.0;
    else
      info->timing.fps = Machine->drv->frames_per_second; /* qbert is 61 fps */

    if ( (Machine->drv->frames_per_second * 1000 < options.samplerate) || ( Machine->drv->frames_per_second < 60) )
    {
      info->timing.sample_rate = Machine->drv->frames_per_second * 1000;
      log_cb(RETRO_LOG_INFO, LOGPRE "Sample timing rate too high for framerate required dropping to %f\n",  Machine->drv->frames_per_second * 1000);
    }

    else
    {
      info->timing.sample_rate = options.samplerate;
      log_cb(RETRO_LOG_INFO, LOGPRE "Sample rate set to %d\n",options.samplerate);
    }
  }

  else
  {
    info->timing.fps = Machine->drv->frames_per_second;

    if ( Machine->drv->frames_per_second * 1000 < options.samplerate)
     info->timing.sample_rate = 22050;

    else
     info->timing.sample_rate = options.samplerate;
  }

}


unsigned retro_api_version(void)
{
  return RETRO_API_VERSION;
}

void retro_get_system_info(struct retro_system_info *info)
{
   /* this must match the 'corename' field in mame2003_libretro.info
    * in order for netplay to work. */
   info->library_name = "MAME 2003 (0.78)";
#ifndef GIT_VERSION
#define GIT_VERSION ""
#endif
   info->library_version = "0.78" GIT_VERSION;
   info->valid_extensions = "zip";
   info->need_fullpath = true;
   info->block_extract = true;
}

#define describe_buttons(INDEX) \
  { INDEX, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,   "Joystick Left" },\
  { INDEX, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT,  "Joystick Right" },\
  { INDEX, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,     "Joystick Up" },\
  { INDEX, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,   "Joystick Down" },\
  { INDEX, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,      "Button 1" },\
  { INDEX, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y,      "Button 2" },\
  { INDEX, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X,      "Button 3" },\
  { INDEX, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,      "Button 4" },\
  { INDEX, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L,      "Button 5" },\
  { INDEX, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R,      "Button 6" },\
  { INDEX, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L2,     "Button 7" },\
  { INDEX, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R2,     "Button 8" },\
  { INDEX, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L3,     "Button 9" },\
  { INDEX, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R3,     "Button 10" },\
  { INDEX, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT, "Insert Coin" },\
  { INDEX, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START,  "Start" },

struct retro_input_descriptor desc[] = {
  describe_buttons(0)
  describe_buttons(1)
  describe_buttons(2)
  describe_buttons(3)
  { 0, 0, 0, 0, NULL }
};

static void remove_slash (char* temp)
{
  int i;

  for(i=0; temp[i] != '\0'; ++i);

  log_cb(RETRO_LOG_INFO, LOGPRE "Check for trailing slash in path: %s\n", temp);

  if( (temp[i-1] == '/' || temp[i-1] == '\\') && (i > 1) )
  {
    temp[i-1] = 0;
    log_cb(RETRO_LOG_INFO, LOGPRE "Removed a trailing slash in path: %s\n", temp);
  }
  else
    log_cb(RETRO_LOG_INFO, LOGPRE "Trailing slash removal was not necessary for path given.\n");
}

bool retro_load_game(const struct retro_game_info *game)
{
  int              driverIndex    = 0;
  int              port_index;
  char             *driver_lookup = NULL;

  if(string_is_empty(game->path))
  {
    log_cb(RETRO_LOG_ERROR, LOGPRE "Content path is not set. Exiting!\n");
    return false;
  }

  log_cb(RETRO_LOG_INFO, LOGPRE "Full content path %s\n", game->path);
  if(!path_is_valid(game->path))
  {
    log_cb(RETRO_LOG_ERROR, LOGPRE "Content path is not valid. Exiting!");
    return false;
  }
  log_cb(RETRO_LOG_INFO, LOGPRE "Git Version %s\n",GIT_VERSION);
  driver_lookup = auto_strdup(path_basename(game->path));
  path_remove_extension(driver_lookup);

  log_cb(RETRO_LOG_INFO, LOGPRE "Content lookup name: %s\n", driver_lookup);

  for (driverIndex = 0; driverIndex < total_drivers; driverIndex++)
  {
    const struct GameDriver *needle = drivers[driverIndex];

    if ( strcasecmp(driver_lookup, needle->name) == 0 )
    {
      log_cb(RETRO_LOG_INFO, LOGPRE "Driver index counter: %d. Matched game driver: %s\n",  driverIndex, needle->name);
      game_driver = needle;
      options.romset_filename_noext = driver_lookup;
      break;
    }
	if(driverIndex == total_drivers -2) // we could fix the total drives in drivers c but the it pointless its taken into account here
	{
      log_cb(RETRO_LOG_ERROR, LOGPRE "Driver index counter: %d. Game driver not found for %s!\n", driverIndex, driver_lookup);
      return false;
	}
 }

   if(!init_game(driverIndex))
    return false;

  set_content_flags();

  options.libretro_content_path = auto_strdup(game->path);
  path_basedir(options.libretro_content_path);

  /* Get system directory from frontend */
  options.libretro_system_path = NULL;
  environ_cb(RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY,&options.libretro_system_path);
  if (options.libretro_system_path == NULL || options.libretro_system_path[0] == '\0')
  {
      log_cb(RETRO_LOG_INFO, LOGPRE "libretro system path not set by frontend, using content path\n");
      options.libretro_system_path = options.libretro_content_path;
  }

  /* Get save directory from frontend */
  options.libretro_save_path = NULL;
  environ_cb(RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY,&options.libretro_save_path);
  if (options.libretro_save_path == NULL || options.libretro_save_path[0] == '\0')
  {
      log_cb(RETRO_LOG_INFO,  LOGPRE "libretro save path not set by frontend, using content path\n");
      options.libretro_save_path = options.libretro_content_path;
  }

  /* Remove trailing slashes for specified systems */
  remove_slash(options.libretro_content_path);
  remove_slash(options.libretro_system_path);
  remove_slash(options.libretro_save_path);

  log_cb(RETRO_LOG_INFO, LOGPRE "content path: %s\n", options.libretro_content_path);
  log_cb(RETRO_LOG_INFO, LOGPRE " system path: %s\n", options.libretro_system_path);
  log_cb(RETRO_LOG_INFO, LOGPRE "   save path: %s\n", options.libretro_save_path);


  init_core_options();
  update_variables(true);
  configure_cyclone_mode(driverIndex);

  environ_cb(RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS, desc);

  if(!run_game(driverIndex))
    return true;

  return false;
}

void retro_reset (void)
{
    machine_reset(); /* use internal core function */
}

/* get pointer axis vector from coord */
int16_t get_pointer_delta(int16_t coord, int16_t *prev_coord)
{
   int16_t delta = 0;
   if (*prev_coord == 0 || coord == 0)
   {
      *prev_coord = coord;
   }
   else
   {
      if (coord != *prev_coord)
      {
         delta = coord - *prev_coord;
         *prev_coord = coord;
      }
   }

   return delta;
}

void retro_run (void)
{
	int i;
	bool pointer_pressed;
	const struct KeyboardInfo *thisInput;
	bool updated = false;

	poll_cb();

	if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE, &updated) && updated)
		update_variables(false);

	if (options.cpu_clock_scale)
	{
		if (cpunum_get_clockscale(0) != options.cpu_clock_scale)
		{
			log_cb(RETRO_LOG_DEBUG, LOGPRE "changing cpu clock scale from %lf to %lf\n",cpunum_get_clockscale(0),options.cpu_clock_scale);
			cpunum_set_clockscale(0, options.cpu_clock_scale);
		}
	}

	/* Keyboard */
	thisInput = retroKeys;
	while(thisInput->name)
	{
		retroKeyState[thisInput->code] = input_cb(0, RETRO_DEVICE_KEYBOARD, 0, thisInput->code);
		thisInput ++;
	}

	for (i = 0; i < 4; i ++)
	{
      unsigned int offset = (i * 18);

      /* Analog joystick */
      analogjoy[i][0] = input_cb(i, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_X);
      analogjoy[i][1] = input_cb(i, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_Y);
      analogjoy[i][2] = input_cb(i, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_X);
      analogjoy[i][3] = input_cb(i, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_Y);

      /* Joystick */
      if (options.rstick_to_btns)
      {
         /* if less than 0.5 force, ignore and read buttons as usual */
         retroJsState[0 + offset] = analogjoy[i][3] >  0x4000 ? 1 : input_cb(i, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B);
         retroJsState[1 + offset] = analogjoy[i][2] < -0x4000 ? 1 : input_cb(i, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y);
      }
      else
      {
         retroJsState[0 + offset] = input_cb(i, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B);
         retroJsState[1 + offset] = input_cb(i, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y);
      }
      retroJsState[2 + offset] = input_cb(i, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT);
      retroJsState[3 + offset] = input_cb(i, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START);
      retroJsState[4 + offset] = input_cb(i, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP);
      retroJsState[5 + offset] = input_cb(i, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN);
      retroJsState[6 + offset] = input_cb(i, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT);
      retroJsState[7 + offset] = input_cb(i, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT);
      if (options.rstick_to_btns)
      {
         retroJsState[8 + offset] = analogjoy[i][2] >  0x4000 ? 1 : input_cb(i, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A);
         retroJsState[9 + offset] = analogjoy[i][3] < -0x4000 ? 1 : input_cb(i, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X);
      }
      else
      {
         retroJsState[8 + offset] = input_cb(i, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A);
         retroJsState[9 + offset] = input_cb(i, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X);
      }
      retroJsState[10 + offset] = input_cb(i, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L);
      retroJsState[11 + offset] = input_cb(i, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R);
      retroJsState[12 + offset] = input_cb(i, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L2);
      retroJsState[13 + offset] = input_cb(i, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R2);
      retroJsState[14 + offset] = input_cb(i, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L3);
      retroJsState[15 + offset] = input_cb(i, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R3);

      if (options.mouse_device)
      {
         if (options.mouse_device == RETRO_DEVICE_MOUSE)
         {
            retroJsState[16 + offset] = input_cb(i, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_LEFT);
            retroJsState[17 + offset] = input_cb(i, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_RIGHT);
            mouse_x[i] = input_cb(i, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_X);
            mouse_y[i] = input_cb(i, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_Y);
         }
         else /* RETRO_DEVICE_POINTER */
         {
            pointer_pressed = input_cb(i, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_PRESSED);
            retroJsState[16 + offset] = pointer_pressed;
            retroJsState[17 + offset] = 0; /* padding */
            mouse_x[i] = pointer_pressed ? get_pointer_delta(input_cb(i, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_X), &prev_pointer_x) : 0;
            mouse_y[i] = pointer_pressed ? get_pointer_delta(input_cb(i, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_Y), &prev_pointer_y) : 0;
         }
      }
      else
      {
         retroJsState[16 + offset] = 0;
         retroJsState[17 + offset] = 0;
      }

      if ( (options.rstick_to_btns) && (options.content_flags[CONTENT_DUAL_JOYSTICK]) )
      {
         retroJsState[21 + offset] = analogjoy[i][2] >  0x4000 ? 1 : input_cb(i, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A);
         retroJsState[20 + offset] = analogjoy[i][3] < -0x4000 ? 1 : input_cb(i, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X);
         retroJsState[18 + offset] = analogjoy[i][3] >  0x4000 ? 1 : input_cb(i, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B);
         retroJsState[19 + offset] = analogjoy[i][2] < -0x4000 ? 1 : input_cb(i, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y);
	    }
      else
	    {
        retroJsState[21 + offset] = analogjoy[i][2] >  0x4000 ? 1 : 0;
        retroJsState[19 + offset] = analogjoy[i][2] < -0x4000 ? 1 : 0;
        retroJsState[18 + offset] = analogjoy[i][3] >  0x4000 ? 1 : 0;
        retroJsState[20 + offset] = analogjoy[i][3] < -0x4000 ? 1 : 0;
	    }
  }

   mame_frame();
}

void retro_unload_game(void)
{
    mame_done();
    /* do we need to be freeing things here? */

    free(options.romset_filename_noext);
}

void retro_deinit(void)
{
#ifdef LOG_PERFORMANCE
   perf_cb.perf_log();
#endif
}

extern size_t state_get_dump_size(void);

size_t retro_serialize_size(void)
{
    return state_get_dump_size();
}

bool retro_serialize(void *data, size_t size)
{
   int cpunum;
	if(  retro_serialize_size() == size  && size   )
	{
		/* write the save state */
		state_save_save_begin(data);

		/* write tag 0 */
		state_save_set_current_tag(0);
		if(state_save_save_continue())
		{
		    return false;
		}

		/* loop over CPUs */
		for (cpunum = 0; cpunum < cpu_gettotalcpu(); cpunum++)
		{
			cpuintrf_push_context(cpunum);

			/* make sure banking is set */
			activecpu_reset_banking();

			/* save the CPU data */
			state_save_set_current_tag(cpunum + 1);
			if(state_save_save_continue())
			    return false;

			cpuintrf_pop_context();
		}

		/* finish and close */
		state_save_save_finish();

		return true;
	}

	return false;
}

bool retro_unserialize(const void * data, size_t size)
{
    int cpunum;
	/* if successful, load it */
	if ( (retro_serialize_size() ) && ( data ) && ( size ) && ( !state_save_load_begin((void*)data, size) ) )
	{
        /* read tag 0 */
        state_save_set_current_tag(0);
        if(state_save_load_continue())
            return false;

        /* loop over CPUs */
        for (cpunum = 0; cpunum < cpu_gettotalcpu(); cpunum++)
        {
            cpuintrf_push_context(cpunum);

            /* make sure banking is set */
            activecpu_reset_banking();

            /* load the CPU data */
            state_save_set_current_tag(cpunum + 1);
            if(state_save_load_continue())
                return false;

            cpuintrf_pop_context();
        }

        /* finish and close */
        state_save_load_finish();


        return true;
	}

	return false;
}

/******************************************************************************

  Sound

  osd_start_audio_stream() is called at the start of the emulation to initialize
  the output stream, then osd_update_audio_stream() is called every frame to
  feed new data. osd_stop_audio_stream() is called when the emulation is stopped.

  The sample rate is fixed at Machine->sample_rate. Samples are 16-bit, signed.
  When the stream is stereo, left and right samples are alternated in the
  stream.

  osd_start_audio_stream() and osd_update_audio_stream() must return the number
  of samples (or couples of samples, when using stereo) required for next frame.
  This will be around Machine->sample_rate / Machine->drv->frames_per_second,
  the code may adjust it by SMALL AMOUNTS to keep timing accurate and to
  maintain audio and video in sync when using vsync. Note that sound emulation,
  especially when DACs are involved, greatly depends on the number of samples
  per frame to be roughly constant, so the returned value must always stay close
  to the reference value of Machine->sample_rate / Machine->drv->frames_per_second.
  Of course that value is not necessarily an integer so at least a +/- 1
  adjustment is necessary to avoid drifting over time.

******************************************************************************/

int osd_start_audio_stream(int stereo)
{
  if (options.machine_timing)
  {
    if ( ( Machine->drv->frames_per_second * 1000 < options.samplerate) || (Machine->drv->frames_per_second < 60) )
      Machine->sample_rate = Machine->drv->frames_per_second * 1000;

    else Machine->sample_rate = options.samplerate;
  }

  else
  {
    if ( Machine->drv->frames_per_second * 1000 < options.samplerate)
      Machine->sample_rate=22050;

    else
      Machine->sample_rate = options.samplerate;
  }

  delta_samples = 0.0f;
  usestereo = stereo ? 1 : 0;

  /* determine the number of samples per frame */
  samples_per_frame = Machine->sample_rate / Machine->drv->frames_per_second;
  orig_samples_per_frame = samples_per_frame;

  if (Machine->sample_rate == 0) return 0;

  samples_buffer = (short *) calloc(samples_per_frame+16, 2 + usestereo * 2);
  if (!usestereo) conversion_buffer = (short *) calloc(samples_per_frame+16, 4);

  return samples_per_frame;
}


int osd_update_audio_stream(INT16 *buffer)
{
	int i,j;
	if ( Machine->sample_rate !=0 && buffer )
	{
   		memcpy(samples_buffer, buffer, samples_per_frame * (usestereo ? 4 : 2));
		if (usestereo)
			audio_batch_cb(samples_buffer, samples_per_frame);
		else
		{
			for (i = 0, j = 0; i < samples_per_frame; i++)
        		{
				conversion_buffer[j++] = samples_buffer[i];
				conversion_buffer[j++] = samples_buffer[i];
		        }
         		audio_batch_cb(conversion_buffer,samples_per_frame);
		}


		//process next frame

		if ( samples_per_frame  != orig_samples_per_frame ) samples_per_frame = orig_samples_per_frame;

		// dont drop any sample frames some games like mk will drift with time

		delta_samples += (Machine->sample_rate / Machine->drv->frames_per_second) - orig_samples_per_frame;
		if ( delta_samples >= 1.0f )
		{

			int integer_delta = (int)delta_samples;
			if (integer_delta <= 16 )
                        {
				log_cb(RETRO_LOG_DEBUG,"sound: Delta added value %d added to frame\n",integer_delta);
				samples_per_frame += integer_delta;
			}
			else if(integer_delta >= 16) log_cb(RETRO_LOG_INFO, "sound: Delta not added to samples_per_frame too large integer_delta: %d\n", integer_delta);
			else log_cb(RETRO_LOG_DEBUG,"sound(delta) no conditions met\n");
			delta_samples -= integer_delta;

		}
	}
        return samples_per_frame;
}


void osd_stop_audio_stream(void)
{
}



/******************************************************************************

Miscellaneous

******************************************************************************/

unsigned retro_get_region (void) {return RETRO_REGION_NTSC;}
void *retro_get_memory_data(unsigned type) {return 0;}
size_t retro_get_memory_size(unsigned type) {return 0;}
bool retro_load_game_special(unsigned game_type, const struct retro_game_info *info, size_t num_info){return false;}
void retro_cheat_reset(void){}
void retro_cheat_set(unsigned unused, bool unused1, const char* unused2){}
void retro_set_video_refresh(retro_video_refresh_t cb) { video_cb = cb; }
void retro_set_audio_sample(retro_audio_sample_t cb) { }
void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb) { audio_batch_cb = cb; }
void retro_set_input_poll(retro_input_poll_t cb) { poll_cb = cb; }
void retro_set_input_state(retro_input_state_t cb) { input_cb = cb; }


/******************************************************************************

	RetroPad mapping

******************************************************************************/

#define EMIT_RETRO_PAD(INDEX) \
    {"RetroPad" #INDEX " Left", ((INDEX - 1) * 18) + RETRO_DEVICE_ID_JOYPAD_LEFT, JOYCODE_##INDEX##_LEFT}, \
    {"RetroPad" #INDEX " Right", ((INDEX - 1) * 18) + RETRO_DEVICE_ID_JOYPAD_RIGHT, JOYCODE_##INDEX##_RIGHT}, \
    {"RetroPad" #INDEX " Up", ((INDEX - 1) * 18) + RETRO_DEVICE_ID_JOYPAD_UP, JOYCODE_##INDEX##_UP}, \
    {"RetroPad" #INDEX " Down", ((INDEX - 1) * 18) + RETRO_DEVICE_ID_JOYPAD_DOWN, JOYCODE_##INDEX##_DOWN}, \
    {"RetroPad" #INDEX " B", ((INDEX - 1) * 18) + RETRO_DEVICE_ID_JOYPAD_B, JOYCODE_##INDEX##_BUTTON1}, \
    {"RetroPad" #INDEX " Y", ((INDEX - 1) * 18) + RETRO_DEVICE_ID_JOYPAD_Y, JOYCODE_##INDEX##_BUTTON2}, \
    {"RetroPad" #INDEX " X", ((INDEX - 1) * 18) + RETRO_DEVICE_ID_JOYPAD_X, JOYCODE_##INDEX##_BUTTON3}, \
    {"RetroPad" #INDEX " A", ((INDEX - 1) * 18) + RETRO_DEVICE_ID_JOYPAD_A, JOYCODE_##INDEX##_BUTTON4}, \
    {"RetroPad" #INDEX " L", ((INDEX - 1) * 18) + RETRO_DEVICE_ID_JOYPAD_L, JOYCODE_##INDEX##_BUTTON5}, \
    {"RetroPad" #INDEX " R", ((INDEX - 1) * 18) + RETRO_DEVICE_ID_JOYPAD_R, JOYCODE_##INDEX##_BUTTON6}, \
    {"RetroPad" #INDEX " L2", ((INDEX - 1) * 18) + RETRO_DEVICE_ID_JOYPAD_L2, JOYCODE_##INDEX##_BUTTON7}, \
    {"RetroPad" #INDEX " R2", ((INDEX - 1) * 18) + RETRO_DEVICE_ID_JOYPAD_R2, JOYCODE_##INDEX##_BUTTON8}, \
    {"RetroPad" #INDEX " L3", ((INDEX - 1) * 18) + RETRO_DEVICE_ID_JOYPAD_L3, JOYCODE_##INDEX##_BUTTON9}, \
    {"RetroPad" #INDEX " R3", ((INDEX - 1) * 18) + RETRO_DEVICE_ID_JOYPAD_R3, JOYCODE_##INDEX##_BUTTON10}, \
    {"RetroPad" #INDEX " Start", ((INDEX - 1) * 18) + RETRO_DEVICE_ID_JOYPAD_START, JOYCODE_##INDEX##_START}, \
    {"RetroPad" #INDEX " Select", ((INDEX - 1) * 18) + RETRO_DEVICE_ID_JOYPAD_SELECT, JOYCODE_##INDEX##_SELECT}, \
    {"RetroMouse" #INDEX " Left Click", ((INDEX - 1) * 18) + 16, JOYCODE_MOUSE_##INDEX##_BUTTON1}, \
    {"RetroMouse" #INDEX " Right Click", ((INDEX - 1) * 18) + 17, JOYCODE_MOUSE_##INDEX##_BUTTON2}

struct JoystickInfo jsItems[] =
{
    EMIT_RETRO_PAD(1),
    EMIT_RETRO_PAD(2),
    EMIT_RETRO_PAD(3),
    EMIT_RETRO_PAD(4),
    {0, 0, 0}
};

/******************************************************************************

	Joystick & Mouse/Trackball

******************************************************************************/

int16_t mouse_x[4];
int16_t mouse_y[4];
int16_t analogjoy[4][4];

void retro_set_controller_port_device(unsigned in_port, unsigned device){}

const struct JoystickInfo *osd_get_joy_list(void)
{
    return jsItems;
}

int osd_is_joy_pressed(int joycode)
{
  if (options.input_interface == RETRO_DEVICE_KEYBOARD)
    return 0;

  return (joycode >= 0) ? retroJsState[joycode] : 0;
}

int osd_is_joystick_axis_code(int joycode)
{
    return 0;
}

void osd_lightgun_read(int player, int *deltax, int *deltay)
{

}

void osd_trak_read(int player, int *deltax, int *deltay)
{
    *deltax = mouse_x[player];
    *deltay = mouse_y[player];
}

#ifdef _MSC_VER
#if _MSC_VER < 1800
double round(double number)
{
  return (number >= 0) ? (int)(number + 0.5) : (int)(number - 0.5);
}
#endif
#endif

int convert_analog_scale(int input)
{
	static const int TRIGGER_MAX = 0x8000;
	int neg_test=0;
	float scale;
	int trigger_deadzone;

	if( options.analog_scale)   trigger_deadzone = (32678 /100) * 20;
	if( !options.analog_scale)  trigger_deadzone = (32678 * options.analog_deadzone) / 100;

	if (input < 0) { input =abs(input); neg_test=1; }
	scale = ((float)TRIGGER_MAX/(float)(TRIGGER_MAX - trigger_deadzone));

	if ( input > 0 && input > trigger_deadzone )
	{
		// Re-scale analog range
		float scaled = (input - trigger_deadzone)*scale;
    input = (int)round(scaled);

		if (input > +32767)
		{
			input = +32767;
		}
		input = input / 326.78;
	}

	else
	{
		input = 0;
	}


	if (neg_test) input =-abs(input);
	return input * 1.28;
}

void osd_analogjoy_read(int player,int analog_axis[MAX_ANALOG_AXES], InputCode analogjoy_input[MAX_ANALOG_AXES])
{
    int i;
    for (i = 0; i < MAX_ANALOG_AXES; i ++)
    {
        if (analogjoy[player][i])
            analog_axis[i] = convert_analog_scale(analogjoy[player][i]);
    }

    analogjoy_input[0] = IPT_AD_STICK_X;
    analogjoy_input[1] = IPT_AD_STICK_Y;
}

void osd_customize_inputport_defaults(struct ipd *defaults)
{

  unsigned int i = 0;
  default_inputs = defaults;

  for( ; default_inputs[i].type != IPT_END; ++i)
  {
    struct ipd *entry = &default_inputs[i];

    if(options.dual_joysticks)
    {
      switch(entry->type)
      {
         case (IPT_JOYSTICKRIGHT_UP   | IPF_PLAYER1):
            seq_set_1(&entry->seq, JOYCODE_2_UP);
            break;
         case (IPT_JOYSTICKRIGHT_DOWN | IPF_PLAYER1):
            seq_set_1(&entry->seq, JOYCODE_2_DOWN);
            break;
         case (IPT_JOYSTICKRIGHT_LEFT | IPF_PLAYER1):
            seq_set_1(&entry->seq, JOYCODE_2_LEFT);
            break;
         case (IPT_JOYSTICKRIGHT_RIGHT | IPF_PLAYER1):
            seq_set_1(&entry->seq, JOYCODE_2_RIGHT);
            break;
         case (IPT_JOYSTICK_UP   | IPF_PLAYER2):
            seq_set_1(&entry->seq, JOYCODE_3_UP);
            break;
         case (IPT_JOYSTICK_DOWN | IPF_PLAYER2):
            seq_set_1(&entry->seq, JOYCODE_3_DOWN);
            break;
         case (IPT_JOYSTICK_LEFT | IPF_PLAYER2):
            seq_set_1(&entry->seq, JOYCODE_3_LEFT);
            break;
         case (IPT_JOYSTICK_RIGHT | IPF_PLAYER2):
            seq_set_1(&entry->seq, JOYCODE_3_RIGHT);
            break;
         case (IPT_JOYSTICKRIGHT_UP   | IPF_PLAYER2):
            seq_set_1(&entry->seq, JOYCODE_4_UP);
            break;
         case (IPT_JOYSTICKRIGHT_DOWN | IPF_PLAYER2):
            seq_set_1(&entry->seq, JOYCODE_4_DOWN);
            break;
         case (IPT_JOYSTICKRIGHT_LEFT | IPF_PLAYER2):
            seq_set_1(&entry->seq, JOYCODE_4_LEFT);
            break;
         case (IPT_JOYSTICKRIGHT_RIGHT | IPF_PLAYER2):
            seq_set_1(&entry->seq, JOYCODE_4_RIGHT);
            break;
         case (IPT_JOYSTICKLEFT_UP   | IPF_PLAYER2):
            seq_set_1(&entry->seq, JOYCODE_3_UP);
            break;
         case (IPT_JOYSTICKLEFT_DOWN | IPF_PLAYER2):
            seq_set_1(&entry->seq, JOYCODE_3_DOWN);
            break;
         case (IPT_JOYSTICKLEFT_LEFT | IPF_PLAYER2):
            seq_set_1(&entry->seq, JOYCODE_3_LEFT);
            break;
         case (IPT_JOYSTICKLEFT_RIGHT | IPF_PLAYER2):
            seq_set_1(&entry->seq, JOYCODE_3_RIGHT);
            break;
     }
    }
   }

}

/* These calibration functions should never actually be used (as long as needs_calibration returns 0 anyway).*/
int osd_joystick_needs_calibration(void) { return 0; }
void osd_joystick_start_calibration(void){ }
const char *osd_joystick_calibrate_next(void) { return 0; }
void osd_joystick_calibrate(void) { }
void osd_joystick_end_calibration(void) { }


/******************************************************************************

	Keyboard

******************************************************************************/

extern const struct KeyboardInfo retroKeys[];
int retroKeyState[512];

const struct KeyboardInfo *osd_get_key_list(void)
{
  return retroKeys;
}

int osd_is_key_pressed(int keycode)
{
	if (options.input_interface == RETRO_DEVICE_JOYPAD)
		return 0;

	if (keycode < 512 && keycode >= 0)
    return retroKeyState[keycode];

  return 0;
}


int osd_readkey_unicode(int flush)
{
  /* TODO*/
  return 0;
}

/******************************************************************************

	Keymapping

******************************************************************************/

/* Unassigned keycodes*/
/*	KEYCODE_OPENBRACE, KEYCODE_CLOSEBRACE, KEYCODE_BACKSLASH2, KEYCODE_STOP, KEYCODE_LWIN, KEYCODE_RWIN, KEYCODE_DEL_PAD, KEYCODE_PAUSE,*/

/* The format for each systems key constants is RETROK_$(TAG) and KEYCODE_$(TAG) */
/* EMIT1(TAG): The tag value is the same between libretro and the core           */
/* EMIT2(RTAG, MTAG): The tag value is different between the two                 */
/* EXITX(TAG): The core has no equivalent key.*/

#define EMIT2(RETRO, KEY) {(char*)#RETRO, RETROK_##RETRO, KEYCODE_##KEY}
#define EMIT1(KEY) {(char*)#KEY, RETROK_##KEY, KEYCODE_##KEY}
#define EMITX(KEY) {(char*)#KEY, RETROK_##KEY, KEYCODE_OTHER}

const struct KeyboardInfo retroKeys[] =
{
    EMIT1(BACKSPACE),
    EMIT1(TAB),
    EMITX(CLEAR),

    EMIT1(BACKSPACE),
    EMIT1(TAB),
    EMITX(CLEAR),
    EMIT2(RETURN, ENTER),
    EMITX(PAUSE),
    EMIT2(ESCAPE, ESC),
    EMIT1(SPACE),
    EMITX(EXCLAIM),
    EMIT2(QUOTEDBL, TILDE),
    EMITX(HASH),
    EMITX(DOLLAR),
    EMITX(AMPERSAND),
    EMIT1(QUOTE),
    EMITX(LEFTPAREN),
    EMITX(RIGHTPAREN),
    EMIT1(ASTERISK),
    EMIT2(PLUS, EQUALS),
    EMIT1(COMMA),
    EMIT1(MINUS),
    EMITX(PERIOD),
    EMIT1(SLASH),

    EMIT1(0), EMIT1(1), EMIT1(2), EMIT1(3), EMIT1(4), EMIT1(5), EMIT1(6), EMIT1(7), EMIT1(8), EMIT1(9),

    EMIT1(COLON),
    EMITX(SEMICOLON),
    EMITX(LESS),
    EMITX(EQUALS),
    EMITX(GREATER),
    EMITX(QUESTION),
    EMITX(AT),
    EMITX(LEFTBRACKET),
    EMIT1(BACKSLASH),
    EMITX(RIGHTBRACKET),
    EMITX(CARET),
    EMITX(UNDERSCORE),
    EMITX(BACKQUOTE),

    EMIT2(a, A), EMIT2(b, B), EMIT2(c, C), EMIT2(d, D), EMIT2(e, E), EMIT2(f, F),
    EMIT2(g, G), EMIT2(h, H), EMIT2(i, I), EMIT2(j, J), EMIT2(k, K), EMIT2(l, L),
    EMIT2(m, M), EMIT2(n, N), EMIT2(o, O), EMIT2(p, P), EMIT2(q, Q), EMIT2(r, R),
    EMIT2(s, S), EMIT2(t, T), EMIT2(u, U), EMIT2(v, V), EMIT2(w, W), EMIT2(x, X),
    EMIT2(y, Y), EMIT2(z, Z),

    EMIT2(DELETE, DEL),

    EMIT2(KP0, 0_PAD), EMIT2(KP1, 1_PAD), EMIT2(KP2, 2_PAD), EMIT2(KP3, 3_PAD),
    EMIT2(KP4, 4_PAD), EMIT2(KP5, 5_PAD), EMIT2(KP6, 6_PAD), EMIT2(KP7, 7_PAD),
    EMIT2(KP8, 8_PAD), EMIT2(KP9, 9_PAD),

    EMITX(KP_PERIOD),
    EMIT2(KP_DIVIDE, SLASH_PAD),
    EMITX(KP_MULTIPLY),
    EMIT2(KP_MINUS, MINUS_PAD),
    EMIT2(KP_PLUS, PLUS_PAD),
    EMIT2(KP_ENTER, ENTER_PAD),
    EMITX(KP_EQUALS),

    EMIT1(UP), EMIT1(DOWN), EMIT1(RIGHT), EMIT1(LEFT),
    EMIT1(INSERT), EMIT1(HOME), EMIT1(END), EMIT2(PAGEUP, PGUP), EMIT2(PAGEDOWN, PGDN),

    EMIT1(F1), EMIT1(F2), EMIT1(F3), EMIT1(F4), EMIT1(F5), EMIT1(F6),
    EMIT1(F7), EMIT1(F8), EMIT1(F9), EMIT1(F10), EMIT1(F11), EMIT1(F12),
    EMITX(F13), EMITX(F14), EMITX(F15),

    EMIT1(NUMLOCK),
    EMIT1(CAPSLOCK),
    EMIT2(SCROLLOCK, SCRLOCK),
    EMIT1(RSHIFT), EMIT1(LSHIFT), EMIT2(RCTRL, RCONTROL), EMIT2(LCTRL, LCONTROL), EMIT1(RALT), EMIT1(LALT),
    EMITX(RMETA), EMITX(LMETA), EMITX(LSUPER), EMITX(RSUPER),

    EMITX(MODE),
    EMITX(COMPOSE),

    EMITX(HELP),
    EMIT2(PRINT, PRTSCR),
    EMITX(SYSREQ),
    EMITX(BREAK),
    EMIT1(MENU),
    EMITX(POWER),
    EMITX(EURO),
    EMITX(UNDO),

    {0, 0, 0}
};

static void configure_cyclone_mode (int driverIndex)
{
  /* Determine how to use cyclone if available to the platform */

#if (HAS_CYCLONE || HAS_DRZ80)
  int i;
  int use_cyclone = 1;
  int use_drz80 = 1;
  int use_drz80_snd = 1;

  /* cyclone mode core option: 0=disabled, 1=default, 2=Cyclone, 3=DrZ80, 4=Cyclone+DrZ80, 5=DrZ80(snd), 6=Cyclone+DrZ80(snd) */
  switch (options.cyclone_mode)
  {
    case 0:
      use_cyclone = 0;
      use_drz80_snd = 0;
      use_drz80 = 0;
      break;

    case 1:
      for (i=0;i<NUMGAMES;i++)
      {
        /* ASM cores: 0=disabled, 1=Cyclone, 2=DrZ80, 3=Cyclone+DrZ80, 4=DrZ80(snd), 5=Cyclone+DrZ80(snd) */
        if (strcmp(drivers[driverIndex]->name,fe_drivers[i].name)==0)
        {
          switch (fe_drivers[i].cores)
          {
            case 0:
              use_cyclone = 0;
              use_drz80_snd = 0;
              use_drz80 = 0;
              break;
            case 1:
              use_drz80_snd = 0;
              use_drz80 = 0;
              break;
            case 2:
              use_cyclone = 0;
              break;
            case 4:
              use_cyclone = 0;
              use_drz80 = 0;
              break;
            case 5:
              use_drz80 = 0;
              break;
            default:
              break;
          }

          break; /* end for loop */
        }
      }
      break; /* end case 1 */

    case 2:
      use_drz80_snd = 0;
      use_drz80 = 0;
      break;

    case 3:
      use_cyclone = 0;
      break;

    case 5:
      use_cyclone = 0;
      use_drz80 = 0;
      break;

    case 6:
      use_drz80 = 0;
      break;

    default:
      break;
  }

#if (HAS_CYCLONE)
  /* Replace M68000 by CYCLONE */
  if (use_cyclone)
  {
    for (i=0;i<MAX_CPU;i++)
    {
      unsigned int *type=(unsigned int *)&(Machine->drv->cpu[i].cpu_type);

#ifdef NEOMAME
      if (*type==CPU_M68000)
#else
      if (*type==CPU_M68000 || *type==CPU_M68010 )
#endif
      {
        *type=CPU_CYCLONE;
        log_cb(RETRO_LOG_INFO, LOGPRE "Replaced CPU_CYCLONE\n");
      }

      if (!(*type)) break;
    }
  }
#endif

#if (HAS_DRZ80)
  /* Replace Z80 by DRZ80 */
  if (use_drz80)
  {
    for (i=0;i<MAX_CPU;i++)
    {
      unsigned int *type=(unsigned int *)&(Machine->drv->cpu[i].cpu_type);
      if (type==CPU_Z80)
      {
        *type=CPU_DRZ80;
        log_cb(RETRO_LOG_INFO, LOGPRE "Replaced Z80\n");
      }
    }
  }

  /* Replace Z80 with DRZ80 only for sound CPUs */
  if (use_drz80_snd)
  {
    for (i=0;i<MAX_CPU;i++)
    {
      int *type=(int*)&(Machine->drv->cpu[i].cpu_type);
      if (type==CPU_Z80 && Machine->drv->cpu[i].cpu_flags&CPU_AUDIO_CPU)
      {
        *type=CPU_DRZ80;
        log_cb(RETRO_LOG_INFO, LOGPRE "Replaced Z80 sound\n");
      }
    }
  }
#endif

#endif
}
