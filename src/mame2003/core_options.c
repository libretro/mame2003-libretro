/*********************************************************************

  core_options.c

  core option configurations used by mame2003

*********************************************************************/

#include "mame.h"
#include "driver.h"
#include <libretro.h>
#include <string/stdstring.h>

bool old_dual_joystick_state = false; /* used to track when this core option changes */

static struct retro_variable  default_options[OPT_end + 1];    /* need the plus one for the NULL entries at the end */
static struct retro_variable  current_options[OPT_end + 1];

/* used in inptport.c when saving input port settings */
int legacy_flag = -1;

/******************************************************************************

  private function prototypes

******************************************************************************/

static void   init_default(struct retro_variable *def, const char *key, const char *value);
static void   set_variables(void);


/******************************************************************************

  external function prototypes and variables

******************************************************************************/

/* found in mame2003.c */
extern void   retro_set_audio_buff_status_cb(void);
extern void   retro_describe_controls(void);

extern const struct GameDriver *game_driver;
extern retro_set_led_state_t led_state_cb;
