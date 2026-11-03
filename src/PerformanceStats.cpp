#include "PerformanceStats.h"
#include <cmath>

using namespace std;

bool PerformanceStats::addFrame(double seconds) {
    if (!isfinite(seconds) || seconds <= 0) return false;
    elapsedSeconds += seconds;
    ++frameCount;
    if (elapsedSeconds < 0.5) return false;
    displayedFps = frameCount / elapsedSeconds;
    displayedFrameMs = 1000 * elapsedSeconds / frameCount;
    elapsedSeconds = 0;
    frameCount = 0;
    return true;
}
