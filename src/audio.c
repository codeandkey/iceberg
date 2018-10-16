#include "audio.h"
#include "mem.h"
#include "log.h"
#include "hashmap.h"

#include "deps/stb_vorbis/stb_vorbis.c"

#include <AL/al.h>
#include <AL/alc.h>

#define IB_AUDIO_BGM_FADEIN 0
#define IB_AUDIO_BGM_PLAYING 1
#define IB_AUDIO_BGM_FADEOUT 2
#define IB_AUDIO_BGM_STOPPED 3

/*
 * it's way more annoying to do refcounting/etc with audio since
 * most of the logic happens on another thread.
 * for now we'll keep all loaded sounds in memory until the subsystem is freed
 */

typedef struct {
    unsigned int buf;
} ib_audio_buffer;

typedef struct {
    ib_audio_source* s;
    int state;
    float vol;
} ib_audio_bgm;

static struct {
    int initialized;
    ib_hashmap* buffer_map, *bg_map;
    ALCdevice* dev;
    ALCcontext* ctx;
} _ib_audio_state;

static int _ib_audio_load_buffer(const char* path);
static void _ib_audio_buffer_free(const char* k, void* v);
static void _ib_audio_update_bgm(const char* k, void* v);

static float _ib_audio_fade_step = 0.0f; /* sneaky global state here, sorry */

int ib_audio_init() {
    if (_ib_audio_state.initialized) return ib_warn("already initialized");

    if (!(_ib_audio_state.dev = alcOpenDevice(NULL))) return ib_err("failed to open audio device");
    _ib_audio_state.ctx = alcCreateContext(_ib_audio_state.dev, NULL);
    if (!alcMakeContextCurrent(_ib_audio_state.ctx)) return ib_err("failed to set audio context");

    _ib_audio_state.buffer_map = ib_hashmap_alloc(256);
    _ib_audio_state.bg_map = ib_hashmap_alloc(256);

    if (alGetError() != AL_NO_ERROR) {
        return ib_err("AL init error");
    }

    ALfloat orientation[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };

    alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
    alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);
    alListenerfv(AL_ORIENTATION, orientation);

    _ib_audio_state.initialized = 1;
    return ib_ok("initialized audio");
}

void ib_audio_free() {
    if (!_ib_audio_state.initialized) {
        ib_warn("not initialized");
        return;
    }

    alcMakeContextCurrent(NULL);
    alcDestroyContext(_ib_audio_state.ctx);
    alcCloseDevice(_ib_audio_state.dev);

    ib_hashmap_foreach(_ib_audio_state.buffer_map, _ib_audio_buffer_free);

    ib_hashmap_free(_ib_audio_state.buffer_map);
    ib_hashmap_free(_ib_audio_state.bg_map);

    _ib_audio_state.initialized = 0;
}

void ib_audio_bgm_add(const char* path) {
    ib_audio_bgm* target = ib_hashmap_get(_ib_audio_state.bg_map, path);

    if (!target) {
        target = ib_malloc(sizeof *target);
        target->s = ib_audio_source_load(path);

        if (!target->s) {
            ib_err("failed to load bgm track");
            return;
        }

        ib_audio_source_loop(target->s, 1);
        target->state = IB_AUDIO_BGM_STOPPED;
        target->vol = 0;
        ib_hashmap_set(_ib_audio_state.bg_map, path, target);
    }

    if (target->state > IB_AUDIO_BGM_PLAYING) {
        target->vol = 0.0f;
        target->state = IB_AUDIO_BGM_FADEIN;

        ib_audio_source_vol(target->s, target->vol);
        ib_audio_source_play(target->s);
        ib_ok("playing %s", path);
    }
}

void ib_audio_bgm_drop(const char* path) {
    ib_audio_bgm* target = ib_hashmap_get(_ib_audio_state.bg_map, path);
    if (!target) return;

    if (target->state != IB_AUDIO_BGM_STOPPED) {
        target->state = IB_AUDIO_BGM_FADEOUT;
    }
}

void ib_audio_update(int dt) {
    _ib_audio_fade_step = (dt * IB_AUDIO_FADE_PER_SECOND) / 1000.0f;
    ib_hashmap_foreach(_ib_audio_state.bg_map, _ib_audio_update_bgm);
}

void _ib_audio_update_bgm(const char* key, void* v) {
    ib_audio_bgm* b = (ib_audio_bgm*) v;

    if (b->state == IB_AUDIO_BGM_FADEIN) {
        if ((b->vol += _ib_audio_fade_step) >= 1.0f) {
            b->vol = 1.0f;
            b->state = IB_AUDIO_BGM_PLAYING;
        }

        ib_audio_source_vol(b->s, b->vol);
    }

    if (b->state == IB_AUDIO_BGM_FADEOUT) {
        if ((b->vol -= _ib_audio_fade_step) <= 0.0f) {
            b->vol = 0.0f;
            b->state = IB_AUDIO_BGM_STOPPED;
        }

        ib_audio_source_vol(b->s, b->vol);
    }
}

static int _ib_audio_load_buffer(const char* path) {
    short* pcm_data;
    int channels = 2, freq = IB_AUDIO_FREQ;
    int pcm_data_len = stb_vorbis_decode_filename(path, &channels, &freq, &pcm_data);

    if (pcm_data_len <= 0) {
        ib_err("failed to load audio from %s", path);
        return 0;
    }

    unsigned int buf;
    alGenBuffers(1, &buf);

    if (!alIsBuffer(buf)) {
        ib_err("AL buffer generation failed");
        return 0;
    }

    int format = ((channels == 2) ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16);
    alBufferData(buf, format, pcm_data, pcm_data_len * channels * 2, freq);

    free(pcm_data);

    if (alGetError() != AL_NO_ERROR) {
        ib_err("error loading buffer data for %s", path);
        return 0;
    }

    return buf;
}

void _ib_audio_buffer_free(const char* k, void* v) {
    ib_audio_buffer* b = (ib_audio_buffer*) v;
    alDeleteBuffers(1, &b->buf);
    ib_free(v);
}

ib_audio_source* ib_audio_source_load(const char* path) {
    int buf;
    ib_audio_buffer* b = ib_hashmap_get(_ib_audio_state.buffer_map, path);

    if (!b) {
        buf = _ib_audio_load_buffer(path);
        if (!buf) return NULL;
        b = ib_malloc(sizeof *b);
        b->buf = buf;

        ib_hashmap_set(_ib_audio_state.buffer_map, path, b);
    }

    buf = b->buf;
    ib_audio_source* out = ib_malloc(sizeof *out);
    out->buf = buf;

    alGenSources(1, &out->source);
    alSourcei(out->source, AL_BUFFER, out->buf);

    return out;
}

void ib_audio_source_free(ib_audio_source* s) {
    alSourceStop(s->source);
    alDeleteSources(1, &s->source);
    ib_free(s);
}

void ib_audio_source_vol(ib_audio_source* s, float vol) {
    alSourcef(s->source, AL_GAIN, vol);
}

void ib_audio_source_play(ib_audio_source* s) {
    alSourcePlay(s->source);
}

void ib_audio_source_stop(ib_audio_source* s) {
    alSourceStop(s->source);
}

void ib_audio_source_loop(ib_audio_source* s, int loop) {
    alSourcei(s->source, AL_LOOPING, loop);
}
