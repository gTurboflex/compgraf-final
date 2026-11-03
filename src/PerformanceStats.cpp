#include "PerformanceStats.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

void PerformanceStats::addFrame(double seconds) {
    if (!std::isfinite(seconds) || seconds <= 0) return;
    displaySeconds += seconds;
    ++displayFrames;
    if (displaySeconds >= 0.5) {
        displayFps = static_cast<double>(displayFrames) / displaySeconds;
        displayFrameMs = 1000 * displaySeconds / static_cast<double>(displayFrames);
        displaySeconds = 0;
        displayFrames = 0;
    }
    if (!measuring || complete) return;
    if (warmupRemaining > 0) {
        warmupRemaining = std::max(0.0, warmupRemaining - seconds);
        return;
    }
    frameTimes.push_back(seconds);
    sampledSeconds += seconds;
    if (sampledSeconds >= sampleTarget) complete = true;
}

void PerformanceStats::beginBenchmark(double warmupSeconds, double sampleSeconds) {
    if (!std::isfinite(warmupSeconds) || warmupSeconds < 0 ||
        !std::isfinite(sampleSeconds) || sampleSeconds <= 0)
        throw std::invalid_argument("Benchmark durations must be finite and sample time must be positive");
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
    std::sort(ordered.begin(), ordered.end());
    result.minFrameMs = ordered.front() * 1000;
    result.maxFrameMs = ordered.back() * 1000;
    const auto index = static_cast<std::size_t>(std::ceil(0.95 * ordered.size())) - 1;
    result.p95FrameMs = ordered[index] * 1000;
    return result;
}
