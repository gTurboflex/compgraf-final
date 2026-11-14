#pragma once
#include <cstddef>
#include <cstdint>

using namespace std;

// Audio-thread-only oscillator state; no allocations or device dependencies.
class AudioSynth {
public:
    void render(float* stereo, size_t frames, bool alarm, bool click);
private:
    double motorPhase = 0, fanPhase = 0, modulation = 0, sirenPhase = 0;
    double clickPhase = 0, clickTime = 1;
    float fanNoise = 0, alarmGain = 0, masterGain = 0;
    uint32_t randomState = 0x18427abc;
};
