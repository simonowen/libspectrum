/* libspectrum.h: the library for dealing with ZX Spectrum emulator files
   Copyright (c) 2001-2002 Philip Kendall, Darren Salt

   $Id$

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

   Author contact information:

   E-mail: pak21-fuse@srcf.ucam.org
   Postal address: 15 Crescent Road, Wokingham, Berks, RG40 2DB, England

*/

#ifndef LIBSPECTRUM_LIBSPECTRUM_H
#define LIBSPECTRUM_LIBSPECTRUM_H

#include <config.h>

#include <stdlib.h>

#include <glib.h>

#if   SIZEOF_CHAR  == 1
typedef unsigned  char libspectrum_byte;
#elif SIZEOF_SHORT == 1
typedef unsigned short libspectrum_byte;
#else
#error No plausible 8 bit types found
#endif

#if   SIZEOF_SHORT == 2
typedef unsigned short libspectrum_word;
#elif SIZEOF_INT   == 2
typedef unsigned   int libspectrum_word;
#else
#error No plausible 16 bit types found
#endif

#if   SIZEOF_INT   == 4
typedef unsigned  int libspectrum_dword;
#elif SIZEOF_LONG  == 4
typedef unsigned long libspectrum_dword;
#else
#error No plausible 32 bit types found
#endif

#ifdef __GNUC__
#define GCC_UNUSED __attribute__ ((unused))
#else				/* #ifdef __GNUC__ */
#define GCC_UNUSED
#endif				/* #ifdef __GNUC__ */

typedef unsigned char uchar;

/*
 * General libspectrum routines
 */

/* The various errors which can occur */
typedef enum libspectrum_error {

  LIBSPECTRUM_ERROR_NONE = 0,
  LIBSPECTRUM_ERROR_MEMORY,
  LIBSPECTRUM_ERROR_UNKNOWN,
  LIBSPECTRUM_ERROR_CORRUPT,
  LIBSPECTRUM_ERROR_SIGNATURE,
  LIBSPECTRUM_ERROR_SLT,	/* .slt data found at end of a .z80 file */

  LIBSPECTRUM_ERROR_LOGIC = -1,

} libspectrum_error;

/* Error handling */

extern int libspectrum_show_errors;
const char* libspectrum_error_message( libspectrum_error error );

/*
 * Snap handling routines
 */

/* The machine types we can handle */
typedef enum libspectrum_machine {

  LIBSPECTRUM_MACHINE_48,
  LIBSPECTRUM_MACHINE_128,
  LIBSPECTRUM_MACHINE_PLUS3,
  LIBSPECTRUM_MACHINE_PENT,

} libspectrum_machine;

typedef enum libspectrum_slt_type {

  LIBSPECTRUM_SLT_TYPE_END = 0,
  LIBSPECTRUM_SLT_TYPE_LEVEL,
  LIBSPECTRUM_SLT_TYPE_INSTRUCTIONS,
  LIBSPECTRUM_SLT_TYPE_SCREEN,
  LIBSPECTRUM_SLT_TYPE_PICTURE,
  LIBSPECTRUM_SLT_TYPE_POKE,

} libspectrum_slt_type;

typedef struct libspectrum_snap {

  /* Which machine are we using here? */

  libspectrum_machine machine;

  /* Registers and the like */

  libspectrum_byte a , f ; libspectrum_word bc , de , hl ;
  libspectrum_byte a_, f_; libspectrum_word bc_, de_, hl_;

  libspectrum_word ix, iy; libspectrum_byte i, r;
  libspectrum_word sp, pc;

  libspectrum_byte iff1, iff2, im;

  /* RAM */

  libspectrum_byte *pages[8];

  /* Data from .slt files */

  libspectrum_byte *slt[256];	/* Level data */
  size_t slt_length[256];	/* Length of each level */

  libspectrum_byte *slt_screen;	/* Loading screen */
  int slt_screen_level;		/* The id of the loading screen. Not used
				   for anything AFAIK, but I'll copy it
				   around just in case */

  /* Peripheral status */

  libspectrum_byte out_ula; libspectrum_dword tstates;

  libspectrum_byte out_128_memoryport;

  libspectrum_byte out_ay_registerport, ay_registers[16];

  libspectrum_byte out_plus3_memoryport;

  /* Internal use only */

  int version;
  int compressed;

} libspectrum_snap;

int libspectrum_snap_initalise( libspectrum_snap *snap );
int libspectrum_snap_destroy( libspectrum_snap *snap );

/* .sna specific routines */

int libspectrum_sna_read( const uchar *buffer, size_t buffer_length,
			  libspectrum_snap *snap );

/* .z80 specific routines */

int libspectrum_z80_read( const libspectrum_byte *buffer, size_t buffer_length,
			  libspectrum_snap *snap );

int libspectrum_z80_write( libspectrum_byte **buffer, size_t *length,
			   libspectrum_snap *snap );

/*
 * Tape handling routines
 */

/* The various types of block available
   The values here must be the same as used in the .tzx format */
typedef enum libspectrum_tape_type {
  LIBSPECTRUM_TAPE_BLOCK_ROM = 0x10,
  LIBSPECTRUM_TAPE_BLOCK_TURBO,
  LIBSPECTRUM_TAPE_BLOCK_PURE_TONE,
  LIBSPECTRUM_TAPE_BLOCK_PULSES,
  LIBSPECTRUM_TAPE_BLOCK_PURE_DATA,
  LIBSPECTRUM_TAPE_BLOCK_RAW_DATA,

  LIBSPECTRUM_TAPE_BLOCK_PAUSE = 0x20,
  LIBSPECTRUM_TAPE_BLOCK_GROUP_START,
  LIBSPECTRUM_TAPE_BLOCK_GROUP_END,
  LIBSPECTRUM_TAPE_BLOCK_JUMP,
  LIBSPECTRUM_TAPE_BLOCK_LOOP_START,
  LIBSPECTRUM_TAPE_BLOCK_LOOP_END,

  LIBSPECTRUM_TAPE_BLOCK_SELECT = 0x28,

  LIBSPECTRUM_TAPE_BLOCK_STOP48 = 0x2a,

  LIBSPECTRUM_TAPE_BLOCK_COMMENT = 0x30,
  LIBSPECTRUM_TAPE_BLOCK_MESSAGE,
  LIBSPECTRUM_TAPE_BLOCK_ARCHIVE_INFO,
  LIBSPECTRUM_TAPE_BLOCK_HARDWARE,

  LIBSPECTRUM_TAPE_BLOCK_CUSTOM = 0x35,

  LIBSPECTRUM_TAPE_BLOCK_CONCAT = 0x5a,
} libspectrum_tape_type;

/* Some flags */
extern const int LIBSPECTRUM_TAPE_FLAGS_BLOCK;	/* End of block */
extern const int LIBSPECTRUM_TAPE_FLAGS_STOP;	/* Stop tape */
extern const int LIBSPECTRUM_TAPE_FLAGS_STOP48;	/* Stop tape if in 48K mode */

/* The states which a block can be in */
typedef enum libspectrum_tape_state_type {

  LIBSPECTRUM_TAPE_STATE_PILOT,	/* Pilot pulses */
  LIBSPECTRUM_TAPE_STATE_SYNC1,	/* First sync pulse */
  LIBSPECTRUM_TAPE_STATE_SYNC2,	/* Second sync pulse */
  LIBSPECTRUM_TAPE_STATE_DATA1,	/* First edge of a data bit */
  LIBSPECTRUM_TAPE_STATE_DATA2,	/* Second edge of a data bit */
  LIBSPECTRUM_TAPE_STATE_PAUSE,	/* The pause at the end of a block */

} libspectrum_tape_state_type;

/* A standard ROM loading block */
typedef struct libspectrum_tape_rom_block {
  
  size_t length;		/* How long is this block */
  libspectrum_byte *data;	/* The actual data */
  libspectrum_dword pause;	/* Pause after block (milliseconds) */

  /* Private data */

  libspectrum_tape_state_type state;

  size_t edge_count;		/* Number of pilot pulses to go */

  size_t bytes_through_block;
  size_t bits_through_byte;	/* How far through the data are we? */

  libspectrum_byte current_byte; /* The current data byte; gets shifted out
				    as we read bits from it */
  libspectrum_dword bit_tstates; /* How long is an edge for the current bit */

} libspectrum_tape_rom_block;

/* The timings for the standard ROM loader */
extern const libspectrum_dword LIBSPECTRUM_TAPE_TIMING_PILOT; /* Pilot */
extern const libspectrum_dword LIBSPECTRUM_TAPE_TIMING_SYNC1; /* Sync 1 */
extern const libspectrum_dword LIBSPECTRUM_TAPE_TIMING_SYNC2; /* Sync 2 */
extern const libspectrum_dword LIBSPECTRUM_TAPE_TIMING_DATA0; /* Reset bit */
extern const libspectrum_dword LIBSPECTRUM_TAPE_TIMING_DATA1; /* Set bit */

/* The number of pilot pulses for the standard ROM loader */
extern const size_t LIBSPECTRUM_TAPE_PILOTS_HEADER;
extern const size_t LIBSPECTRUM_TAPE_PILOTS_DATA;

/* A turbo loading block */
typedef struct libspectrum_tape_turbo_block {

  size_t length;		/* Length of data */
  size_t bits_in_last_byte;	/* How many bits are in the last byte? */
  libspectrum_byte *data;	/* The actual data */
  libspectrum_dword pause;	/* Pause after data (in ms) */

  libspectrum_dword pilot_length; /* Length of pilot pulse (in tstates) */
  size_t pilot_pulses;		/* Number of pilot pulses */

  libspectrum_dword sync1_length, sync2_length; /* Length of the sync pulses */
  libspectrum_dword bit0_length, bit1_length; /* Length of (re)set bits */

  /* Private data */

  libspectrum_tape_state_type state;

  size_t edge_count;		/* Number of pilot pulses to go */

  size_t bytes_through_block;
  size_t bits_through_byte;	/* How far through the data are we? */

  libspectrum_byte current_byte; /* The current data byte; gets shifted out
				    as we read bits from it */
  libspectrum_dword bit_tstates; /* How long is an edge for the current bit */

} libspectrum_tape_turbo_block;

/* A pure tone block */
typedef struct libspectrum_tape_pure_tone_block {

  libspectrum_dword length;
  size_t pulses;

  /* Private data */

  size_t edge_count;

} libspectrum_tape_pure_tone_block;

/* A list of pulses of different lengths */
typedef struct libspectrum_tape_pulses_block {

  size_t count;
  libspectrum_dword *lengths;

  /* Private data */

  size_t edge_count;

} libspectrum_tape_pulses_block;

/* A block of just of data */
typedef struct libspectrum_tape_pure_data_block {

  size_t length;		/* Length of data */
  size_t bits_in_last_byte;	/* How many bits are in the last byte? */
  libspectrum_byte *data;	/* The actual data */
  libspectrum_dword pause;	/* Pause after data (in ms) */

  libspectrum_dword bit0_length, bit1_length; /* Length of (re)set bits */

  /* Private data */

  libspectrum_tape_state_type state;

  size_t bytes_through_block;
  size_t bits_through_byte;	/* How far through the data are we? */

  libspectrum_byte current_byte; /* The current data byte; gets shifted out
				    as we read bits from it */
  libspectrum_dword bit_tstates; /* How long is an edge for the current bit */

} libspectrum_tape_pure_data_block;

/* A raw data block */
typedef struct libspectrum_tape_raw_data_block {

  size_t length;		/* Length of data */
  size_t bits_in_last_byte;	/* How many bits are in the last byte? */
  libspectrum_byte *data;	/* The actual data */
  libspectrum_dword pause;	/* Pause after data (in ms) */

  libspectrum_dword bit_length; /* Bit length. *Not* pulse length! */

  /* Private data */

  libspectrum_tape_state_type state;

  size_t bytes_through_block;
  size_t bits_through_byte;	/* How far through the data are we? */

  libspectrum_byte last_bit;	/* The last bit which was read */
  libspectrum_dword bit_tstates; /* How long is an edge for the current bit */

} libspectrum_tape_raw_data_block;

/* A pause block */
typedef struct libspectrum_tape_pause_block {

  libspectrum_dword length;

} libspectrum_tape_pause_block;

/* A group start block */
typedef struct libspectrum_tape_group_start_block {

  libspectrum_byte *name;

} libspectrum_tape_group_start_block;

/* No group end block needed as it contains no data */

/* A jump block */
typedef struct libspectrum_tape_jump_block {

  int offset;

} libspectrum_tape_jump_block;

/* A loop start block */
typedef struct libspectrum_tape_loop_start_block {

  int count;

} libspectrum_tape_loop_start_block;

/* No loop end block needed as it contains no data */

/* A select block */
typedef struct libspectrum_tape_select_block {

  /* Number of selections */
  size_t count;

  /* Offset of each selection, and a description of each */
  int *offsets;
  libspectrum_byte **descriptions;

} libspectrum_tape_select_block;

/* No `stop tape if in 48K mode' block as it contains no data */

/* A comment block */
typedef struct libspectrum_tape_comment_block {

  libspectrum_byte *text;

} libspectrum_tape_comment_block;

/* A message block */
typedef struct libspectrum_tape_message_block {

  int time;
  libspectrum_byte *text;

} libspectrum_tape_message_block;

/* An archive info block */
typedef struct libspectrum_tape_archive_info_block {

  /* Number of strings */
  size_t count;

  /* ID for each string */
  int *ids;

  /* Text of each string */
  libspectrum_byte **strings;

} libspectrum_tape_archive_info_block;

/* A hardware info block */
typedef struct libspectrum_tape_hardware_block {

  /* Number of entries */
  size_t count;

  /* For each entry, a type, an ID and a value */
  int *types, *ids, *values;

} libspectrum_tape_hardware_block;

/* A custom block */
typedef struct libspectrum_tape_custom_block {

  /* Description of this block */
  char description[17];

  /* And the data for it; currently, no attempt is made to interpret
     this data */
  size_t length; libspectrum_byte *data;

} libspectrum_tape_custom_block;

/* No block needed for concatenation block, as it isn't stored */

/* A generic tape block */
typedef struct libspectrum_tape_block {

  libspectrum_tape_type type;

  union {
    libspectrum_tape_rom_block rom;
    libspectrum_tape_turbo_block turbo;
    libspectrum_tape_pure_tone_block pure_tone;
    libspectrum_tape_pulses_block pulses;
    libspectrum_tape_pure_data_block pure_data;
    libspectrum_tape_raw_data_block raw_data;

    libspectrum_tape_pause_block pause;
    libspectrum_tape_group_start_block group_start;
    /* No group end block needed as it contains no data */
    libspectrum_tape_jump_block jump;
    libspectrum_tape_loop_start_block loop_start;
    /* No loop end block needed as it contains no data */

    libspectrum_tape_select_block select;

    /* No `stop tape if in 48K mode' block as it contains no data */

    libspectrum_tape_comment_block comment;
    libspectrum_tape_message_block message;
    libspectrum_tape_archive_info_block archive_info;
    libspectrum_tape_hardware_block hardware;

    libspectrum_tape_custom_block custom;
  } types;

} libspectrum_tape_block;

/* A linked list of tape blocks */
typedef struct libspectrum_tape {

  /* All the blocks */
  GSList* blocks;

  /* The current block */
  GSList* current_block;

  /* Private data */

  /* Where to return to after a loop, and how many iterations of the loop
     to do */
  GSList* loop_block;
  size_t loop_count;

} libspectrum_tape;

/* Routines to manipulate tape blocks */

libspectrum_error
libspectrum_tape_free( libspectrum_tape *tape );

libspectrum_error
libspectrum_tape_init_block( libspectrum_tape_block *block );

libspectrum_error
libspectrum_tape_get_next_edge( libspectrum_tape *tape,
				libspectrum_dword *tstates, int *flags );

libspectrum_error
libspectrum_tape_block_description( libspectrum_tape_block *block,
				    char *buffer, size_t length );

/*** Routines for .tap format files ***/

libspectrum_error
libspectrum_tap_create( libspectrum_tape *tape, const libspectrum_byte *buffer,
			const size_t length );

/*** Routines for .tzx format files ***/

libspectrum_error
libspectrum_tzx_create( libspectrum_tape *tape, const libspectrum_byte *buffer,
			const size_t length );

libspectrum_error
libspectrum_tzx_write( libspectrum_tape *tape,
		       libspectrum_byte **buffer, size_t *length );

typedef enum libspectrum_rzx_block_t {

  LIBSPECTRUM_RZX_CREATOR_BLOCK = 0x10,
  LIBSPECTRUM_RZX_SNAPSHOT_BLOCK = 0x30,
  LIBSPECTRUM_RZX_INPUT_BLOCK = 0x80,

} libspectrum_rzx_block_t;

typedef struct libspectrum_rzx_frame_t {

  size_t instructions;

  size_t count;
  libspectrum_byte* in_bytes;

  int repeat_last;			/* Set if we should use the last
					   frame's value */

} libspectrum_rzx_frame_t;

typedef struct libspectrum_rzx {

  libspectrum_rzx_frame_t *frames;
  size_t count;
  size_t allocated;

  size_t tstates;

} libspectrum_rzx;

libspectrum_error libspectrum_rzx_frame( libspectrum_rzx *rzx,
					 size_t instructions, 
					 size_t count,
					 libspectrum_byte *in_bytes );

libspectrum_error libspectrum_rzx_free( libspectrum_rzx *rzx );

libspectrum_error
libspectrum_rzx_read( libspectrum_rzx *rzx, const libspectrum_byte *buffer,
		      const size_t length, libspectrum_snap **snap );

libspectrum_error
libspectrum_rzx_write( libspectrum_rzx *rzx,
		       libspectrum_byte **buffer, size_t *length,
		       libspectrum_byte *snap, size_t snap_length,
		       const char *program, libspectrum_word major,
		       libspectrum_word minor, int compress );

#endif				/* #ifndef LIBSPECTRUM_LIBSPECTRUM_H */
