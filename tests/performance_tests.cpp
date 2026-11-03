#include "PerformanceStats.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <cassert>
#include <cmath>
#include <initializer_list>
#include <limits>

using namespace std;

bool close(double a, double b) { return abs(a - b) < 1e-8; }

int main() {
    for (int rate : {30, 144}) {
        PerformanceStats stats;
        int updates = 0;
        for (int frame = 0; frame < rate * 2; ++frame)
            if (stats.addFrame(1.0 / rate)) ++updates;
        assert(updates >= 2);
        assert(close(stats.fps(), rate));
        assert(close(stats.frameMs(), 1000.0 / rate));
    }

    PerformanceStats variable;
    assert(!variable.addFrame(0.125));
    assert(!variable.addFrame(0.125));
    assert(variable.addFrame(0.25));
    assert(close(variable.fps(), 6));
    assert(close(variable.frameMs(), 1000.0 / 6));

    PerformanceStats filtered;
    assert(!filtered.addFrame(0.25));
    assert(!filtered.addFrame(0));
    assert(!filtered.addFrame(-1));
    assert(!filtered.addFrame(numeric_limits<double>::quiet_NaN()));
    assert(!filtered.addFrame(numeric_limits<double>::infinity()));
    assert(filtered.addFrame(0.25));
    assert(close(filtered.fps(), 4));
    assert(close(filtered.frameMs(), 250));

    for (int frame = 0; frame < 3; ++frame) {
        assert(!filtered.addFrame(0.125));
        assert(close(filtered.fps(), 4));
    }
    assert(filtered.addFrame(0.125));
    assert(close(filtered.fps(), 8));
    assert(close(filtered.frameMs(), 125));
}
