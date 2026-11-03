# Topic 27 — Server Room
Member 3 integration: per-fragment Blinn–Phong, two cold-blue ceiling point lights,
independently blinking emissive LED quads, and flashing red alarm mode.
The primitive room is an integration demonstration for the team's future scene.

## Build (macOS)
Install Xcode command line tools and dependencies:
```sh
brew install cmake glfw glm
cmake -S . -B build
cmake --build build -j
./build/OpenGLApp
```
Shader paths are resolved from the source directory at configure time. Keep the
source checkout available when running this development build.
A toggles alarm; L toggles ceiling lights; Escape exits. Keys toggle once per press.
`./build/OpenGLApp --smoke-test` renders for about three seconds, checks the FPS
display, and reports shader or OpenGL errors through a nonzero exit.

## Integration
Lighting.h exposes toggleAlarm(), toggleCeiling(), and apply().
ServerEffects.h uploads time and a stable unique LED ID. Assign one ID to each
LED; do not generate a new ID each frame. Both shader programs use model/view/
projection uniforms; lit geometry uses location 0 position and location 1 normal.
See docs/lighting.md for equations and verification.
Other team requirements (camera controls, full scene hierarchy, FPS
overlay, report and video) remain outside this Member 3 contribution.
## Member 5 — Textures and atmosphere

The floor, walls, rack frames and perforated front panels each have a repeating
surface map and independent Blinn–Phong parameters. Press F to toggle cold-air
fog. Compare the floor near and far from the camera, then switch A to see the
fog and LEDs respond to alarm mode. Texture paths, like shader paths, are set
at configure time. The four surface maps are bundled; Python is only needed to
regenerate them.

See [Textures and atmosphere](docs/textures-and-atmosphere.md) for the report
section and the mesh integration contract. AI use: OpenAI Codex generated the
Member 5 code, surface maps and documentation based on the assigned task.

## Member 4 - UI and FPS

This stage adds an on-screen help panel, scene status, FPS, and frame time.
Press H to hide the longer help list; FPS stays visible. The panel lists the
current controls: A for alarm, L for ceiling lights, F for fog, and Esc to exit.
Camera movement and mouse look are planned for the next stage.

The FPS display updates about twice per second using frames divided by elapsed
time. A benchmark waits two seconds, then records a 30-second sample at the
current window size. It disables VSync and saves the actual OpenGL renderer.

```sh
ctest --test-dir build --output-on-failure
./build/OpenGLApp --benchmark normal --output normal.json
./build/OpenGLApp --benchmark alarm --output alarm.json
./build/OpenGLApp --benchmark ceiling-off --output ceiling-off.json
```

On Windows, use `.\build\OpenGLApp.exe` with the same options. The original
development build requires GLFW and GLM, and loads assets from the source
checkout. See [UI and FPS results](docs/ui-fps.md) for the measured run.

