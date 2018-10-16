/*
 * audio manager
 * works automatically with fading tracks in/out and sound effects
 */

#ifndef IB_AUDIO
#define IB_AUDIO

#define IB_AUDIO_AUDIOFILE(x) "res/sound/" x ".ogg"

#define IB_AUDIO_FADE_PER_SECOND 0.2
#define IB_AUDIO_FREQ 44100

typedef struct {
    unsigned int source, buf;
} ib_audio_source;

int ib_audio_init();
void ib_audio_free();

/* for playing sound effects, possibly repeatedly */
ib_audio_source* ib_audio_source_load(const char* path);
void ib_audio_source_play(ib_audio_source* s);
void ib_audio_source_vol(ib_audio_source* s, float v);
void ib_audio_source_loop(ib_audio_source* s, int loop);
void ib_audio_source_free(ib_audio_source* s);

void ib_audio_bgm_add(const char* path); /* starts a looping bgm track, will fade in if it isn't already playing */
void ib_audio_bgm_drop(const char* path);

void ib_audio_update(int dt);

#endif
