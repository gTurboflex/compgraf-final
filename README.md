# Topic 27 — Server Room
Lighting integration: per-fragment Blinn–Phong, two cold-blue ceiling point lights,
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
`./build/OpenGLApp --smoke-test` renders four frames across all lighting modes
and reports shader compilation/link or OpenGL errors through a nonzero exit.

## Integration
Lighting.h exposes toggleAlarm(), toggleCeiling(), and apply().
ServerEffects.h uploads time and a stable unique LED ID. Assign one ID to each
LED; do not generate a new ID each frame. Both shader programs use model/view/
projection uniforms; lit geometry uses location 0 position and location 1 normal.
See docs/lighting.md for equations and verification.
Other team requirements (camera controls, full scene hierarchy, FPS
overlay, report and video) remain outside this lighting contribution.
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

