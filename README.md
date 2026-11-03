# Topic 27: Server Room

A C++17 and OpenGL 3.3 room with server racks, blinking LEDs, cold blue lights,
and a red alarm mode. Member 3 provides lighting and LED effects. Member 5
provides surface maps and fog. Member 4 adds walking, mouse look, door controls,
an on-screen help panel, and FPS measurements.

The current primitive room is the team's integration scene. The full team
report, slides, final video, and final submission tag are separate deliverables.

## Run a packaged build

Extract the whole preview ZIP, then open `OpenGLApp.exe` on Windows. Keep the
`shaders` and `assets` folders beside it. The package includes the runtime files;
the source checkout is not needed to run it. An OpenGL 3.3 driver is required.

## Build

GLFW 3.4 and GLM 1.0.1 are included in `external`, with their licenses. CMake
does not download packages during configuration. You need a C++17 compiler,
CMake 3.16 or newer, and Ninja when using MSYS2.

Windows, from PowerShell in the project folder:

```powershell
.\scripts\build-windows.ps1 -Package
.\build\bin\OpenGLApp.exe
```

The script finds MSYS2 UCRT64 in common folders. Use `-ToolchainBin` for another
location, or run it from a Visual Studio developer terminal without MSYS2.

macOS, with Xcode command line tools and CMake:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
./build/bin/OpenGLApp
```

Windows and macOS builds are also defined in `.github/workflows/build.yml`.
Each successful job uploads a preview ZIP. The release is a Member 4 preview,
not the team's final `v1.0` submission.

## Controls

Click the scene to capture the mouse. The letter controls use GLFW key events,
so the normal English and Russian keyboard layouts use the same key positions.

| Input | Action |
|---|---|
| W / A / S / D | Walk forward / left / back / right |
| Mouse | Look around |
| Tab | Release or capture the mouse |
| Left click | Capture the mouse |
| B | Toggle the red overheat alarm |
| E | Open or close the nearest rack door within 2.3 units |
| L | Toggle ceiling lights |
| F | Toggle the existing cold-air fog |
| H | Show or hide help; FPS stays visible |
| R | Reset the camera |
| Escape | Exit |

Alarm moved from A to B because A is now part of WASD. Toggling happens once
per press. Movement stays on the floor plane. Collision is not implemented;
the camera can pass through objects. When focus is lost, walking stops and the
mouse is released. A bottom prompt shows whether a door is close enough.

## Member 4 files

- `Camera`: position, yaw, pitch, movement, view and projection matrices.
- `Input`: key events, mouse look, capture and focus handling.
- `RackDoors.h`: nearby-door selection and a simple hinged door in this scene.
- `UI`: help, mode labels, door prompt, FPS and frame time.
- `PerformanceStats`: rolling display and timed benchmark samples.

`main.cpp` connects these modules to the existing lighting and materials.
It sends the same camera matrices to lit surfaces and LEDs. The viewport and
projection use the current framebuffer size, including after a resize.

## Tests and measurements

```powershell
ctest --test-dir build -C Release --output-on-failure
.\build\bin\OpenGLApp.exe --smoke-test
.\build\bin\OpenGLApp.exe --benchmark normal --seconds 30 --output normal.json
.\build\bin\OpenGLApp.exe --benchmark alarm --seconds 30 --output alarm.json
.\build\bin\OpenGLApp.exe --benchmark ceiling-off --seconds 30 --output ceiling-off.json
```

Benchmark mode disables VSync, waits two seconds, then measures the requested
sample time. It records actual framebuffer dimensions, GPU renderer, mean FPS,
p95 frame time, and the slowest frame. Minimized periods are skipped. A run
closed early is marked incomplete. `--capture frame.ppm` saves a real OpenGL
frame after the FPS display has updated; no pointer is drawn into that file.

The smoke test opens a real window for about four seconds. Keep it focused
and do not press keys during the test. It checks walking through the installed
callbacks, nearby-door opening and closing, scene toggles, FPS, and GL errors.

See [Member 4 implementation and results](docs/member4.md),
[the Russian code explanation](docs/member4-explanation-ru.md), and
[the video plan](docs/member4-video.md). Raw measurements are in `docs/evidence`.

## Team integration and credits

Keep one owner for the GLFW user pointer and callbacks: the `Input` object.
Connect future scene doors through the same nearest-door action. The current
door frame is a small integration model; it does not replace a future detailed
rack model. Lighting still uses `Lighting::toggleAlarm()` and `toggleCeiling()`.
See [lighting](docs/lighting.md), [textures](docs/textures-and-atmosphere.md),
and [credits](CREDITS.md).
