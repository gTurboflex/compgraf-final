#include "Audio.h"
#include <chrono>
#include <thread>
#include <iostream>

using namespace std;
int main() {
    Audio audio;
    if (!audio.available()) return 1;
    this_thread::sleep_for(chrono::seconds(1));
    audio.playClick();
    audio.setAlarm(true);
    this_thread::sleep_for(chrono::seconds(2));
    audio.playClick();
    audio.setAlarm(false);
    this_thread::sleep_for(chrono::seconds(1));
    cout << "Audio device: ambient, click, alarm on/off completed.\n";
}
