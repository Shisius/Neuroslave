
#include "neuroslave_state.h"

nsv_state_t nsv_set_state(nsv_state_t state, NeuroslaveStateBits state_bit, bool value)
{
	if (value)
		return state | (1 << state_bit);
	else
		return state & ~(1 << state_bit);
}

bool nsv_get_state(nsv_state_t state, NeuroslaveStateBits state_bit)
{
	return (state >> state_bit) & 1;
}

bool nsv_state_session_enabled(nsv_state_t state) 
{
	return nsv_get_state(state, NSV_STATE_SOURCE_READY_SESSION) && nsv_get_state(state, NSV_STATE_HANDLER_READY_SESSION) && nsv_get_state(state, NSV_STATE_SESSION);
}

bool nsv_state_record_enabled(nsv_state_t state)
{
	return nsv_get_state(state, NSV_STATE_MUSIC_READY_RECORD) && nsv_get_state(state, NSV_STATE_HANDLER_READY_RECORD) && nsv_get_state(state, NSV_STATE_RECORD);
}
