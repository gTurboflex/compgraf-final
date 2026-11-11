#include "AudioSynth.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <cassert>
#include <cmath>
#include <vector>
#include <algorithm>

using namespace std;

int main() {
    AudioSynth normal, alarm;
    vector<float> base(96000), active(96000);
    normal.render(base.data(), 48000, false, false);
    alarm.render(active.data(), 48000, true, true);
    double baseEnergy = 0, alarmEnergy = 0;
    for (size_t i = 0; i < base.size(); ++i) {
        assert(isfinite(active[i]) && abs(active[i]) < .8f);
        baseEnergy += base[i] * base[i];
        alarmEnergy += active[i] * active[i];
    }
    assert(baseEnergy > 1 && alarmEnergy > baseEnergy * 3);
    // Removing the alarm fades all the way back to the exact ambient signal.
    normal.render(base.data(), 48000, false, false);
    alarm.render(active.data(), 48000, false, false);
    for (size_t i = 72000; i < base.size(); ++i)
        assert(abs(base[i] - active[i]) < 1e-5f);
    // Click is audible, short, and does not alter the background phase.
    AudioSynth clicked, untouched;
    clicked.render(active.data(), 48000, false, true);
    untouched.render(base.data(), 48000, false, false);
    double difference = 0;
    for (size_t i = 0; i < 6000; ++i) difference += abs(active[i] - base[i]);
    assert(difference > .1);
    for (size_t i = 7000; i < base.size(); ++i) assert(active[i] == base[i]);
    // Callback block size must not change the sound or introduce loop seams.
    AudioSynth whole, chunks;
    whole.render(base.data(), 48000, true, false);
    for (size_t i = 0; i < 48000; i += 100)
        chunks.render(active.data() + 2*i, 100, true, false);
    assert(base == active);
}
