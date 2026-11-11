#include "PerformanceStats.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <cassert>
#include <cmath>
#include <initializer_list>
#include <limits>
#include <stdexcept>

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

    PerformanceStats sample;
    sample.beginBenchmark(0.1, 0.04);
    sample.addFrame(0.1);
    assert(sample.benchmark().frames == 0);
    sample.addFrame(0.01);
    sample.addFrame(0);
    sample.addFrame(numeric_limits<double>::infinity());
    assert(!sample.benchmarkDone());
    sample.addFrame(0.03);
    assert(sample.benchmarkDone());
    auto result = sample.benchmark();
    assert(result.frames == 2);
    assert(close(result.meanFps, 50));
    assert(close(result.minFrameMs, 10));
    assert(close(result.maxFrameMs, 30));
    assert(close(result.p95FrameMs, 30));
    sample.addFrame(1);
    assert(sample.benchmark().frames == 2);
    sample.beginBenchmark(0, 1);
    assert(!sample.benchmarkDone() && sample.benchmark().frames == 0);
    sample.addFrame(0.01);
    assert(!sample.benchmarkDone() && sample.benchmark().frames == 1);

    for (double invalid : {-1.0, numeric_limits<double>::infinity(),
                           numeric_limits<double>::quiet_NaN()}) {
        bool rejected = false;
        try { sample.beginBenchmark(invalid, 1); }
        catch (const invalid_argument&) { rejected = true; }
        assert(rejected);
    }
    for (double invalid : {0.0, -1.0, numeric_limits<double>::infinity(),
                           numeric_limits<double>::quiet_NaN()}) {
        bool rejected = false;
        try { sample.beginBenchmark(0, invalid); }
        catch (const invalid_argument&) { rejected = true; }
        assert(rejected);
    }
    sample.beginBenchmark(0, 0.21);
    for (int i = 1; i <= 20; ++i) sample.addFrame(i / 1000.0);
    assert(sample.benchmarkDone());
    assert(close(sample.benchmark().p95FrameMs, 19));
}
