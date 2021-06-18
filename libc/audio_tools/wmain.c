
#include "wave_music.h"

int main() {
	WaveMusic wave_music;
	if (openWaveMusic(&wave_music, "/home/swk/Music/LostInHazeStory.wav")){
		printf("Music opened\n");
	} else {
		printf("Music open failed\n");
	}
	wave_music.play(&wave_music);
	while (wave_music.is_playing(&wave_music)) {
		printf("Still playing\n");
		sleep(1);
	}
	closeWaveMusic(&wave_music);

	return 0;
}
