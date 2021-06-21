
#include "wave_music.h"

int main() {
	WaveMusic wave_music;
	if (openWaveMusic(&wave_music, "/home/pi/BCI/wav.wav")){
		printf("Music opened\n");
	} else {
		printf("Music open failed\n");
		return -1;
	}
	wave_music.play(&wave_music);
	while (wave_music.is_playing(&wave_music)) {
		printf("Still playing\n");
		sleep(1);
	}
	closeWaveMusic(&wave_music);

	return 0;
}
