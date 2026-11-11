#include "Audio.h"
#include "AudioSynth.h"
#include <miniaudio.h>
#include <atomic>
#include <iostream>

using namespace std;

struct Audio::Impl {
    ma_device device{};
    AudioSynth synth;
    atomic<bool> alarm{false};
    atomic<bool> click{false};
    bool initialized = false, started = false;

    static void callback(ma_device* device, void* output, const void*, ma_uint32 frames) {
        auto& self = *static_cast<Impl*>(device->pUserData);
        self.synth.render(static_cast<float*>(output), frames,
                          self.alarm.load(), self.click.exchange(false));
    }

    Impl() {
        auto config = ma_device_config_init(ma_device_type_playback);
        config.playback.format = ma_format_f32;
        config.playback.channels = 2;
        config.sampleRate = 48000;
        config.dataCallback = callback;
        config.pUserData = this;
        initialized = ma_device_init(nullptr, &config, &device) == MA_SUCCESS;
        if (initialized) started = ma_device_start(&device) == MA_SUCCESS;
        if (!started) cerr << "Audio unavailable; continuing without sound.\n";
    }
    ~Impl() {
        // Uninitialization waits for the callback before destroying its state.
        if (initialized) ma_device_uninit(&device);
    }
};

Audio::Audio() : impl(make_unique<Impl>()) {}
Audio::~Audio() = default;
void Audio::setAlarm(bool enabled) { impl->alarm.store(enabled); }
void Audio::playClick() { impl->click.store(true); }
bool Audio::available() const { return impl->started; }
