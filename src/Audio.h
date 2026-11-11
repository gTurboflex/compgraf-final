#pragma once
#include <memory>

using namespace std;

class Audio {
public:
    Audio();
    ~Audio();
    Audio(const Audio&) = delete;
    Audio& operator=(const Audio&) = delete;
    void setAlarm(bool enabled);
    void playClick();
    bool available() const;
private:
    struct Impl;
    unique_ptr<Impl> impl;
};
