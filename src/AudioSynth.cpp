#include "AudioSynth.h"
#include <cmath>
#include <algorithm>

using namespace std;

namespace {
constexpr double pi = 3.14159265358979323846;
constexpr double rate = 48000.0;
void advancePhase(double& phase, double frequency) {
    phase += 2 * pi * frequency / rate;
    if (phase >= 2 * pi) phase -= 2 * pi;
}
}

void AudioSynth::render(float* stereo, size_t frames, bool alarm, bool click) {
    if (click) clickTime = 0;
    for (size_t i = 0; i < frames; ++i) {
        randomState = randomState * 1664525u + 1013904223u;
        float noise = float(randomState >> 8) / 8388608.f - 1.f;
        fanNoise += .045f * (noise - fanNoise);
        advancePhase(motorPhase, 60);
        advancePhase(fanPhase, 121.3);
        advancePhase(modulation, .8);
        float hum = .045f * float(sin(motorPhase)) +
                    .018f * float(sin(2 * motorPhase)) +
                    .013f * float(sin(fanPhase)) + .09f * fanNoise;
        hum *= float(.94 + .06 * sin(modulation));
        // Continuous phase and a 40 ms envelope prevent pops on alarm toggles.
        advancePhase(sirenPhase, 760 + 240 * sin(modulation));
        alarmGain += ((alarm ? 1.f : 0.f) - alarmGain) * (1.f / 1920.f);
        float siren = alarmGain * .15f *
            float(sin(sirenPhase) + .18 * sin(3 * sirenPhase));
        float button = 0;
        if (clickTime < .065) {
            advancePhase(clickPhase, 1500 - 600 * clickTime / .065);
            button = .13f * float(sin(clickPhase) * sin(pi * clickTime / .065) *
                                  exp(-40 * clickTime));
            clickTime += 1 / rate;
        }
        masterGain += (1.f - masterGain) / 4800.f;
        float sample = masterGain * (hum + siren + button);
        stereo[2 * i] = stereo[2 * i + 1] = clamp(sample, -.8f, .8f);
    }
}
