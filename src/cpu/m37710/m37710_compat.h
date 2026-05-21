/*
 * m37710_compat.h
 *
 * Compatibility shim that lets the M37710 CPU core build inside mame2003's
 * cpuintrf without rewriting the 5000+ lines of opcode logic. It maps the
 * device/memory/timer API the core was written against onto mame2003's
 * equivalents. Only the thin interface section of m37710.c is adapted by
 * hand; everything the opcode bodies touch is provided here.
 *
 * Held to C89: no // comments, no mixed declarations, no for-loop decls.
 */
#ifndef M37710_COMPAT_H
#define M37710_COMPAT_H

#include "driver.h"
#include "cpuintrf.h"
#include "mamedbg.h"
#include "state.h"
#include "timer.h"

/* mame2003's INLINE is a bare "inline" (no static). Under C99/C11 inline rules
   a bare-inline function defined in a header emits no out-of-line copy, so any
   call the compiler does not inline becomes an undefined symbol at link time.
   The opcode core (m37710op.h) declares all its helpers with bare INLINE, so
   force real, emitted functions instead of relying on inlining. */
#undef INLINE
#define INLINE static

/* ---- basic types the core expects --------------------------------------- */
/* The core (descended from the g65816) uses a lowercase "uint" throughout. */
typedef unsigned int m37710_uint_t;
#define uint m37710_uint_t

/* ---- address space identification --------------------------------------- */
/* The core holds two "const address_space *" (program, io) and hands them to
   the byte accessors. We make address_space a tiny tag the accessor switches
   on rather than a real memory object. The single global instance points its
   program/io fields at these two tags (see m37710.c interface section). */
#define M37710_AS_PROGRAM 0
#define M37710_AS_IO      1

typedef int address_space;

extern const address_space m37710_space_program;
extern const address_space m37710_space_io;

/* ADDRESS_SPACE_* / AS_* tags referenced by the (disabled) internal map and
   by the CPU_INIT body. */
#ifndef AS_PROGRAM
#define AS_PROGRAM M37710_AS_PROGRAM
#endif
#ifndef AS_IO
#define AS_IO M37710_AS_IO
#endif

/* ---- memory access (pure macros; no inline functions) ------------------- */
/* M37710: 16-bit data bus, 24-bit address, little-endian. The _8le byte names
   are used only for the on-chip I/O ports; the _16le byte/word names are always
   program space. All accessors route straight to the 24lew handlers as macros.
   Call-site args are side-effect free, so the multiple references are safe. */
#define memory_read_byte_8le(space, address) \
	(((space) != NULL && *(space) == M37710_AS_IO) \
		? (UINT8)cpu_readport24lew(address) \
		: (UINT8)cpu_readmem24lew(address))

#define memory_write_byte_8le(space, address, data) \
	(((space) != NULL && *(space) == M37710_AS_IO) \
		? (void)cpu_writeport24lew((address), (data)) \
		: (void)cpu_writemem24lew((address), (data)))

#define memory_read_byte_16le(space, address)        memory_read_byte_8le((space), (address))
#define memory_write_byte_16le(space, address, data) memory_write_byte_8le((space), (address), (data))

/* 16-bit word access (program space only). Aligned -> the 24lew word handler;
   odd -> two byte cycles, matching the M37710's unaligned word access. */
#define memory_read_word_16le(space, address) \
	(((address) & 1) \
		? (UINT16)(cpu_readmem24lew(address) | (cpu_readmem24lew((address) + 1) << 8)) \
		: (UINT16)cpu_readmem24lew_word(address))

#define memory_write_word_16le(space, address, data) \
	(((address) & 1) \
		? (void)(cpu_writemem24lew((address), (UINT8)((data) & 0xff)), \
		         cpu_writemem24lew((address) + 1, (UINT8)(((data) >> 8) & 0xff))) \
		: (void)cpu_writemem24lew_word((address), (data)))

/* Per-instruction debug hook: no integrated debugger in this build. */
#define debugger_instruction_hook(device, pc)        ((void)0)


/* ---- device model ------------------------------------------------------- */
/* There is exactly one C76 per board, so the device object collapses to a
   single global cpu instance. These types exist only as opaque pointer
   targets; the few real member accesses (device->space, device->machine) are
   rewritten in the m37710.c interface section. */
typedef struct _m37710_legacy_device legacy_cpu_device;
typedef struct _m37710_legacy_device running_device;
typedef struct _m37710_legacy_machine running_machine;

/* mame2003's IRQ callback is int(*)(int irqline); the core stores one. */
typedef int (*device_irq_callback)(int irqline);

/* get_safe_token ignores the device and returns the one global instance. The
   token is returned as void* and cast at the call site, where the concrete
   m37710i_cpu_struct type is in scope. */
extern void *m37710_get_token(void);
#define get_safe_token(device) ((m37710i_cpu_struct *)m37710_get_token())

/* Device-registration boilerplate from the newer device model: not needed. */
#define DECLARE_LEGACY_CPU_DEVICE(BASENAME, name)

/* ---- CPU interface entry-point macros ----------------------------------- */
/* The core declares its interface bodies with mame's CPU_* function macros.
   mame2003 wires up its own explicit cpu_interface functions (the glue at the
   end of m37710.c), so these only need to expand to compilable static function
   headers carrying the parameter names the bodies reference (device,
   irqcallback, buffer, pc, oprom). The unused params draw warnings only. */
#define CPU_INIT(name)        void cpu_init_##name(legacy_cpu_device *device, device_irq_callback irqcallback)
#define CPU_RESET(name)       void cpu_reset_##name(legacy_cpu_device *device)
#define CPU_EXIT(name)        void cpu_exit_##name(legacy_cpu_device *device)
#define CPU_EXECUTE(name)     void cpu_execute_##name(legacy_cpu_device *device)
#define CPU_DISASSEMBLE(name) int  cpu_disassemble_##name(legacy_cpu_device *device, char *buffer, offs_t pc, const UINT8 *oprom, const UINT8 *opram, int options)

/* ---- attotime -> double seconds ----------------------------------------- */
/* The core only uses attotime to schedule its internal timers. mame2003 timers
   take a double (seconds), so attotime collapses to a double. We keep a
   seconds/attoseconds shape so the (now no-op) save-state lines still compile. */
typedef struct _m37710_attotime
{
	double seconds;
	double attoseconds;
} attotime;

#define ATTOTIME_IN_HZ(hz)        m37710_attotime_in_hz((double)(hz))
#define attotime_mul(a, factor)   m37710_attotime_mul((a), (double)(factor))
#define attotime_to_double(a)     ((a).seconds + (a).attoseconds)

INLINE attotime m37710_attotime_in_hz(double hz)
{
	attotime t;
	t.seconds = (hz != 0.0) ? (1.0 / hz) : 0.0;
	t.attoseconds = 0.0;
	return t;
}

INLINE attotime m37710_attotime_mul(attotime a, double factor)
{
	attotime t;
	t.seconds = (a.seconds + a.attoseconds) * factor;
	t.attoseconds = 0.0;
	return t;
}

/* ---- timers ------------------------------------------------------------- */
/* mame2003 timer callbacks are void(int param) with no ptr; the global cpu
   instance is referenced directly inside the callback. TIMER_CALLBACK expands
   to that signature. timer_alloc/adjust map onto mame2003's API. */
#define TIMER_CALLBACK(name) void name(int param)

/* mame2003's timer object is mame_timer. */
typedef mame_timer emu_timer;

#define m37710_timer_alloc(cb) timer_alloc(cb)
#define timer_adjust_oneshot(which, duration, param) \
	timer_adjust((which), attotime_to_double(duration), (param), 0.0)

/* ---- save state (deferred; no-op for the core-compile milestone) -------- */
#define state_save_register_device_item(dev, idx, val)        ((void)0)
#define state_save_register_device_item_array(dev, idx, val)  ((void)0)
#define state_save_register_postload(machine, cb, param)      ((void)0)
#define STATE_POSTLOAD(name) void name(void *param)

/* ---- misc --------------------------------------------------------------- */
/* The core kicks its own IRQ via cpu_triggerint(device); mame2003 takes a cpu
   index. The active cpu is the C76 when its timer fires, so use the current
   cpu. Rewritten at the call site to drop the device argument. */

/* ---- disassembler return flags (mame2003 dasm returns a length only) ---- */
#ifndef DASMFLAG_SUPPORTED
#define DASMFLAG_STEP_OVER  0
#define DASMFLAG_STEP_OUT   0
#define DASMFLAG_SUPPORTED  0
#endif

/* Generic debug "PC base" state id used as a get_reg case label; no mame2003
   equivalent, so map it to an out-of-range value that never collides with the
   small M37710_* register/line enums. */
#ifndef STATE_GENPCBASE
#define STATE_GENPCBASE     0x40000
#endif

#endif /* M37710_COMPAT_H */
