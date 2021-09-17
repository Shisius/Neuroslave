#ifndef _WAVE_PLAYER_H_
#define _WAVE_PLAYER_H_

extern "C" {
	#include "wave_music.h"
	#include "neuroslave_state.h"
}
#include "neuroslave_struct.h"
#include "neuroslave_fs.h"
#include <atomic>
#include <thread>
#include <chrono>

class WavePlayer
{
protected:
	// Communication
	std::atomic<nsv_state_t> * d_state;
	NeuroslaveSession * d_session;
	NeuroslaveMusic * d_music;
	NeuroslaveGame * d_game;

	// Objects
	WaveMusic d_wave_music;

	// Internal
	std::thread d_music_thread;
	unsigned int d_idle_wait_us = 1000;
	unsigned int d_music_wait_us = 10;

	// Methods
	void music_process();
	bool prepare_music();
	bool prepare_game();
	bool stop_music();

public:	

	WavePlayer(std::atomic<nsv_state_t> * state, NeuroslaveSession * session, NeuroslaveMusic * music, NeuroslaveGame * game);
	~WavePlayer();

	bool start();
	void stop();

};

#endif // _WAVE_PLAYER_H_
