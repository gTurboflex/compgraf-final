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
`./build/OpenGLApp --smoke-test` renders for about 1.2 seconds and checks that
the FPS counter updates. Shader and OpenGL errors cause a nonzero exit.

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

## Member 4 - FPS calculation

This first stage adds a small FPS counter. The window title shows FPS and
average frame time, updated about twice per second. FPS is the number of frames
divided by their elapsed time. Zero, negative, and non-finite time values are
ignored. Rendering and timing pause while the window is minimized.

The in-scene help panel, benchmark files, walking camera, mouse look, and door
controls are for later stages. Current controls remain A, L, F, and Esc.

Build as above, then run `ctest --test-dir build --output-on-failure` to check
the FPS calculation. The tests cover different frame rates, variable frame
times, invalid inputs, and consecutive measurement intervals.

