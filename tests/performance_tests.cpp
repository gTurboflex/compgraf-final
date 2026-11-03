#include "PerformanceStats.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <cassert>
#include <cmath>
#include <limits>
#include <stdexcept>

int main() {
    PerformanceStats stats;
    for (int i = 0; i < 120; ++i) stats.addFrame(1.0 / 60.0);
    assert(std::abs(stats.fps() - 60) < 1e-9);
    assert(std::abs(stats.frameMs() - 1000.0 / 60) < 1e-9);
    stats.addFrame(-1);
    stats.addFrame(std::numeric_limits<double>::quiet_NaN());
    assert(std::abs(stats.fps() - 60) < 1e-9);
    stats.beginBenchmark(0.1, 0.04);
    stats.addFrame(0.1);
    assert(stats.benchmark().frames == 0);
    stats.addFrame(0.01);
    stats.addFrame(0.03);
    assert(stats.benchmarkDone());
    auto result = stats.benchmark();
    assert(result.frames == 2);
    assert(std::abs(result.meanFps - 50) < 1e-9);
    assert(result.minFrameMs == 10 && result.maxFrameMs == 30 && result.p95FrameMs == 30);
    stats.addFrame(1);
    assert(stats.benchmark().frames == 2);
    stats.beginBenchmark(0, 1);
    assert(!stats.benchmarkDone() && stats.benchmark().frames == 0);
    bool rejected = false;
    try { stats.beginBenchmark(-1, 1); } catch (const std::invalid_argument&) { rejected = true; }
    assert(rejected);
}
