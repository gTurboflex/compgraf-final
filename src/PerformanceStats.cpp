#include "PerformanceStats.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

using namespace std;

bool PerformanceStats::addFrame(double seconds) {
    if (!isfinite(seconds) || seconds <= 0) return false;
    bool updated = false;
    elapsedSeconds += seconds;
    ++frameCount;
    if (elapsedSeconds >= 0.5) {
        displayedFps = static_cast<double>(frameCount) / elapsedSeconds;
        displayedFrameMs = 1000 * elapsedSeconds / static_cast<double>(frameCount);
        elapsedSeconds = 0;
        frameCount = 0;
        updated = true;
    }
    if (!measuring || complete) return updated;
    if (warmupRemaining > 0) {
        warmupRemaining = max(0.0, warmupRemaining - seconds);
        return updated;
    }
    frameTimes.push_back(seconds);
    sampledSeconds += seconds;
    if (sampledSeconds >= sampleTarget) complete = true;
    return updated;
}

void PerformanceStats::beginBenchmark(double warmupSeconds, double sampleSeconds) {
    if (!isfinite(warmupSeconds) || warmupSeconds < 0 ||
        !isfinite(sampleSeconds) || sampleSeconds <= 0)
        throw invalid_argument("Benchmark durations must be finite and sample time must be positive");
    measuring = true;
    complete = false;
    warmupRemaining = warmupSeconds;
    sampleTarget = sampleSeconds;
    sampledSeconds = 0;
    frameTimes.clear();
}

BenchmarkResult PerformanceStats::benchmark() const {
    BenchmarkResult result;
    result.frames = frameTimes.size();
    result.seconds = sampledSeconds;
    if (frameTimes.empty()) return result;
    result.meanFps = static_cast<double>(result.frames) / sampledSeconds;
    auto ordered = frameTimes;
    sort(ordered.begin(), ordered.end());
    result.minFrameMs = ordered.front() * 1000;
    result.maxFrameMs = ordered.back() * 1000;
    const auto index = static_cast<size_t>(ceil(0.95 * ordered.size())) - 1;
    result.p95FrameMs = ordered[index] * 1000;
    return result;
}
