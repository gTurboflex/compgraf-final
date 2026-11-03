#pragma once

class PerformanceStats {
public:
    bool addFrame(double seconds);
    double fps() const { return displayedFps; }
    double frameMs() const { return displayedFrameMs; }

private:
    double elapsedSeconds = 0;
    unsigned frameCount = 0;
    double displayedFps = 0;
    double displayedFrameMs = 0;
};
