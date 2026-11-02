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
`./build/OpenGLApp --smoke-test` renders four frames across all lighting modes
and reports shader compilation/link or OpenGL errors through a nonzero exit.

## Integration
Lighting.h exposes toggleAlarm(), toggleCeiling(), and apply().
ServerEffects.h uploads time and a stable unique LED ID. Assign one ID to each
LED; do not generate a new ID each frame. Both shader programs use model/view/
projection uniforms; lit geometry uses location 0 position and location 1 normal.
See docs/lighting.md for equations and verification.
Other team requirements (camera controls, full scene hierarchy, textures, FPS
overlay, report and video) remain outside this Member 3 contribution.
