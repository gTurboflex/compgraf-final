#pragma once

#include <cstddef>
#include <vector>

struct BenchmarkResult {
    std::size_t frames = 0;
    double seconds = 0;
    double meanFps = 0;
    double minFrameMs = 0;
    double maxFrameMs = 0;
    double p95FrameMs = 0;
};

class PerformanceStats {
public:
    void addFrame(double seconds);
    double fps() const { return displayFps; }
    double frameMs() const { return displayFrameMs; }
    void beginBenchmark(double warmupSeconds = 2, double sampleSeconds = 30);
    bool benchmarkDone() const { return complete; }
    BenchmarkResult benchmark() const;

private:
    double displaySeconds = 0;
    std::size_t displayFrames = 0;
    double displayFps = 0;
    double displayFrameMs = 0;
    bool measuring = false;
    bool complete = false;
    double warmupRemaining = 0;
    double sampleTarget = 30;
    double sampledSeconds = 0;
    std::vector<double> frameTimes;
};
