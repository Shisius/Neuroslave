#ifndef _NEUROSLAVE_STATE_H_
#define _NEUROSLAVE_STATE_H_

#include <stdint.h>
#include <stdbool.h>

typedef enum {
	NSV_STATE_ALIVE = 0,
	NSV_STATE_SOURCE_READY_SESSION,
	NSV_STATE_HANDLER_READY_SESSION,
	NSV_STATE_SESSION,
	NSV_STATE_HANDLER_READY_RECORD,
	NSV_STATE_MUSIC_READY_RECORD,
	NSV_STATE_RECORD,
	NSV_STATE_GAME,
	NSV_STATE_SOURCE_FAILED,
	NSV_STATE_HANDLER_FAILED,
	NSV_STATE_MUSIC_FAILED
} NeuroslaveStateBits;

typedef uint32_t nsv_state_t;

nsv_state_t nsv_set_state(nsv_state_t state, NeuroslaveStateBits state_bit, bool value);

bool nsv_get_state(nsv_state_t state, NeuroslaveStateBits state_bit);

bool nsv_state_session_enabled(nsv_state_t state);

bool nsv_state_record_enabled(nsv_state_t state);

#endif //_NEUROSLAVE_STATE_H_
