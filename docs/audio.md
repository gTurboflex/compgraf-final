# Server-room sound

Audio owns a miniaudio playback device and shuts it down before freeing callback
state. The callback generates 48 kHz stereo float samples; miniaudio handles the
output device's native format. No microphone or network access is used.

- Ambient: 60 Hz motor hum, its 120 Hz harmonic, a slightly detuned fan component,
  and filtered pseudorandom airflow noise with slow amplitude variation.
- Button feedback: a 65 ms falling electronic tone with a smooth envelope.
  GLFW press events trigger A/L/F/H sounds; repeat and release events are ignored.
- Alarm: a continuous 520–1000 Hz sweep with a quiet third harmonic. An atomic
  alarm flag follows Lighting::alarm; a 40 ms envelope smooths switching.
- Mixing: quiet fixed gains, a startup fade, and an output safety clamp.
  Ambient sound remains active while ceiling lights or fog are switched off.

Audio and AudioSynth are separate from lighting and rendering. Control events
publish only atomic flags; oscillator state stays on the audio thread. No
allocations, file reads, locks or rendering calls occur in the audio callback.

## Verification

CTest includes audio_tests: finite bounded output, audible ambient and alarm
energy, alarm decay back to ambient, click duration, and independence from audio
callback block size. These tests do not require a sound card.

Run build/audio_device_check (Windows: build/Release/audio_device_check.exe)
for a four-second hardware check: hum, click plus alarm, click plus hum.
The test fails if a playback device cannot start.

Manual integration: start OpenGLApp, hear the hum, press A twice to start and
stop the siren, and press L/F/H to hear button tones. Hold H to confirm only one
tone occurs. Close the application and confirm sound stops. Device failure
leaves graphics usable with a console warning.

Validated on macOS: build, CTest, and playback-device initialization/run.
Windows/Linux playback has not been tested in this change.
