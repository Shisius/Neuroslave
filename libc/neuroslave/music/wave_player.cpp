#include "wave_player.h"

WavePlayer::WavePlayer(std::atomic<nsv_state_t> * state, NeuroslaveSession * session, NeuroslaveMusic * music, NeuroslaveGame * game)
{
	;
}

WavePlayer::~WavePlayer()
{
	;
}

bool WavePlayer::start()
{
	d_music_thread = std::thread(&WavePlayer::music_process, this);
	return true;
}

void WavePlayer::stop()
{
	if (d_music_thread.joinable())
		d_music_thread.join();
}

bool WavePlayer::prepare_game()
{
	return true;
}

bool WavePlayer::prepare_music()
{
	return true;
}

void WavePlayer::music_process()
{
	while (nsv_get_state(d_state->load(std::memory_order_relaxed), NSV_STATE_ALIVE)) {
		// Wait for record
		if (nsv_get_state(d_state->load(std::memory_order_relaxed), NSV_STATE_RECORD)) {
			// Game or just music?
			if (nsv_get_state(d_state->load(std::memory_order_relaxed), NSV_STATE_GAME)) {
				// Game
				if (prepare_game()) {
				}
			} else {
				// Music
				if (prepare_music()) {
				}
			}
			// Wait for others or cancel

		} else {
			std::this_thread::sleep_for(std::chrono::microseconds(d_idle_wait_us));
		}

	}
}
