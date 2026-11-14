# Server Room

Final project for **Computer Graphics Fundamentals**, topic 27. A real-time
server room built with C++17 and OpenGL 3.3 core. Walk between the racks,
open their doors, and switch cold blue lighting to a flashing red overheat
alarm. The scene includes six racks, ceiling fixtures, cooling equipment,
a static monitor, a wall panel, cable trays, textured tiles and cold-air fog.

Team: **Ергун, Даниал, Нурадилет, Бекасыл, Даурен**.

## Dependencies

CMake downloads and builds GLFW and GLM automatically.
The first configure requires internet access; later builds reuse the files
in `build/_deps`. Versions and archive hashes are pinned in
[`cmake/Dependencies.cmake`](cmake/Dependencies.cmake).

| Dependency | How it is provided |
| --- | --- |
| GLFW 3.5.1 | Downloaded from GLFW's official source archive and built as a static library. |
| GLM 1.0.3 | Downloaded from GLM's official source archive and used as headers. |
| GLAD 2, OpenGL 3.3 loader | Pregenerated `deps/glad/gl.h` from the pinned GLFW archive, compiled by `cmake/glad.c`. |
| Khronos/KHR definitions | Embedded in that generated GLAD header; no separate library is needed. |
| miniaudio 0.11.23 | Header, implementation and license are bundled in `external/miniaudio`. |

The loader follows [GLFW's own example setup](https://www.glfw.org/docs/latest/quick_guide.html).
All machines use these same dependency sources, even if other versions are
installed globally.

A clean PC still needs **CMake 3.24+**, a **C/C++ compiler and platform SDK**,
and graphics drivers supporting **OpenGL 3.3**. CMake manages project libraries;
it does not install the compiler, operating-system SDK, or graphics drivers.

Get the project, then follow the instructions for your operating system:

```sh
git clone https://github.com/gTurboflex/compgraf-final.git
cd compgraf-final
```

## Build (macOS)

Install Xcode command line tools and CMake if missing:

```sh
xcode-select --install
brew install cmake
```

From the repository root:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
./build/OpenGLApp
```

## Build (Windows, PowerShell)

Prerequisites: CMake 3.24 or newer, and Visual Studio 2022 or newer with
the **Desktop development with C++** workload (including MSVC and a Windows
SDK). If CMake is missing, install it with:

```powershell
winget install --id Kitware.CMake -e
```

Use Visual Studio Installer to add the C++ workload if needed, then open a new
PowerShell terminal so installed tools are on `PATH`.

From the repository root, configure, build, and run:

```powershell
cmake -S . -B build -A x64
cmake --build build --config Release --parallel
.\build\Release\OpenGLApp.exe
```

For later builds, only the `cmake --build` command is needed. Verify with:

```powershell
ctest --test-dir build -C Release --output-on-failure
.\build\Release\OpenGLApp.exe --smoke-test
```

## Controls

The scene shows a help panel, FPS, frame time, and alarm/light/fog status.
Press **H** to hide or show the help lines. FPS and status stay visible.

| Key | Action |
| --- | --- |
| W / A / S / D | Walk forward / left / back / right |
| Mouse | Look around |
| Tab | Release or capture the mouse |
| Left click in the window | Capture the mouse again |
| B | Toggle the alarm |
| E | Open or close the nearest rack door within 2.3 units |
| L | Toggle the ceiling lights |
| F | Toggle fog |
| H | Show or hide help |
| R | Reset camera position and direction |
| Esc | Close the window |

Walking uses elapsed frame time and keeps a fixed eye height. Diagonal movement
has the same speed as straight movement. Resize updates the projection aspect
ratio. Changing window focus releases the mouse and clears held keys.

## Scene and rendering

Per-fragment Blinn–Phong shading combines ambient, diffuse and specular
terms from two ceiling lights. Server LEDs are emissive quads with different
shader-driven blink timings. Alarm mode changes the room from cold blue to
flashing red. Materials sample repeating textures, and distance/height fog
adds a shallow cold-air layer.

The scene renders six instances of `src/Rack.cpp` in two rows facing a central
aisle. `src/Scene.cpp` keeps their state and shares the indexed cube mesh.
Each rack contains a frame, server units, LED quads, and a hinged
door frame. Metal and panel materials use the existing textures. The door's
`toggle()` / `update(dt)` API is connected to E and the frame loop. The bottom
prompt shows the nearest door and the available action.

The room also contains two ceiling fixtures, a cooling unit with ventilation
louvers, a monitor on a stand, and a wall-mounted control panel. These models
reuse the same cube mesh and the team's textures. The monitor display is static.
Cable trays run above both rack rows, with rails, crosspieces, ceiling supports
and three cable runs. A rear tray connects the rows. Together with the existing
tiled-floor texture, they form the cable-tray/floor stretch feature; fog remains
controlled by F.

`Geometry.cpp` builds 24 cube vertices and 36 indices with outward-facing
triangle winding. Each vertex has a position, normal and UV. The lit shader
scales face UVs by the model dimensions, so long tray rails and rotated racks
keep the existing material repeat scale.
Parts reuse the same mesh with different model matrices; this is mesh reuse,
not GPU instancing. A door's matrix is the rack transform multiplied by the
hinge translation, animated rotation, part translation and scale. Normals use
the inverse transpose of the model matrix to support nonuniform scaling.

## Performance

To measure the scene, run one command at a time. Each run skips two seconds
and then measures 30 seconds at 1280x720 with VSync off:

```powershell
.\build\Release\OpenGLApp.exe --benchmark normal --output build/normal.json
.\build\Release\OpenGLApp.exe --benchmark alarm --output build/alarm.json
.\build\Release\OpenGLApp.exe --benchmark ceiling-off --output build/ceiling-off.json
```

For Ninja builds use `build/OpenGLApp.exe`; on macOS use `./build/OpenGLApp`.
Do not resize the window or change scene settings during a benchmark.
Results include the actual OpenGL renderer, mean FPS, p95 frame time, and
whether the sample finished. Normal use has VSync on.

## Verification

After building on macOS:

```sh
ctest --test-dir build --output-on-failure
./build/OpenGLApp --smoke-test
./build/OpenGLApp --scene-smoke
./build/audio_device_check
```

With Visual Studio on Windows, add `-C Release` to the CTest command and use
`.\build\Release\OpenGLApp.exe` / `.\build\Release\audio_device_check.exe`.
Keep the application focused during the smoke checks. They cover keyboard
and mouse callbacks, walking, independent doors, scene toggles, HUD state
restoration, window resizing and OpenGL errors. The audio-device check plays
the hum, click and siren.

On 14 November 2026, merged code `681f0e6` passed a full Release rebuild, all
five CTest targets, both runtime smoke checks and the audio-device check on
macOS 27.0.1, Apple A18 Pro, 8 GiB RAM. The following sequential measurements
used the starting view, 1280x720, fog and help on, VSync off, a two-second
warmup and a 30-second sample:

| Mode | Mean FPS | p95 frame time | Longest frame |
| --- | ---: | ---: | ---: |
| Normal | 1076.441 | 3.498 ms | 20.605 ms |
| Alarm | 1344.865 | 0.984 ms | 11.705 ms |

These are local measurements, not results from the TA machine. Normal use
enables VSync and keeps the high-resolution framebuffer on Retina displays.
Automated runs use framebuffer pixels for the stated 1280x720 resolution.
After the header-folder reorganization in `f20a952`, the Release build, all
five CTest targets and both runtime smoke checks passed again on the same Mac.

## Project layout

```text
cmake/          Dependency versions, download rules, and the small GLAD adapter
src/            Application C++ source files
include/        Project headers and runtime smoke-test helpers in include/tests/
shaders/        GLSL shader programs
assets/         Runtime textures
external/       Bundled miniaudio source and license
tests/          Automated tests
.vscode/        Shared editor settings
README.md       Setup, controls, features, roles and verification
CREDITS.md      Library licenses, asset origins and technical references
CMakeLists.txt  Application and test build targets
build/          Generated files only; ignored by Git
  _deps/        Downloaded library sources and their build files
```

Deleting `build/` resets all build output and cached downloads; CMake recreates them on the next configure.
Keep new generated output under `build/` rather than adding folders at the root.

CMake records shader and texture paths from the source folder. Keep that
folder available and configure again if you move the project; copying only
the executable is not a portable release package. If a dependency download
is interrupted, check the connection and repeat the configure command.

## Team responsibilities

GitHub accounts identify each member's contributions in the commit history.
Each member supplies their own report section, slides and feature demonstration.
The course report, presentation and video are prepared separately from the
source repository.

| Member | Account | Responsibility |
| --- | --- | --- |
| 1 — Danial Khussainov | [Casper-242464](https://github.com/Casper-242464) | Application loop, Shader/Mesh, CMake, build instructions and release |
| 2 — Yergun Yessentemirov | [gTurboflex](https://github.com/gTurboflex) | Room and rack models, layout, hierarchy, door animation, geometry/UVs, cable trays and slide assembly |
| 3 — Bekassyl Zhaksylyk | [bekasyl00](https://github.com/bekasyl00) | Lighting and LED shaders, alarm effects and synthesized audio |
| 4 — Nuradilet Mustafa | [ad1let-1](https://github.com/ad1let-1) | Camera, input, scene interactions, HUD, performance checks and demo-video assembly |
| 5 — Dauren Nagumash | [daikipeek](https://github.com/daikipeek) | Textures, materials, floor tiles, fog, credits and report assembly |

## Current limitations

- The camera can pass through objects; collision detection is not implemented.
- The monitor contains static status bars, not a live FPS graph.
- Fog approximates a shallow cold-air layer. LEDs are emissive surfaces and
  do not light nearby geometry. Shadows, bloom and physical cable simulation
  are outside the current scope.
- The expanded scene still needs a final build and performance check on the
  TA machine using the course assignment setup.

## Credits

See [CREDITS.md](CREDITS.md) for dependencies, licenses and asset origins.

## AI use

OpenAI Codex assisted with project planning and generated the procedural
textures credited in CREDITS.md. On 11 November 2026, Codex repaired duplicated
code from the latest merge and integrated the Rack model with CMake, the
indexed Mesh renderer, and existing materials. Codex ran the Windows Release
build, CTest, and OpenGL smoke test, and inspected a framebuffer capture.
These automated checks do not replace team review or testing on the TA machine.
On 13–14 November, assistance also covered the expanded room and cable trays,
UV and triangle-winding corrections, verification, and documentation updates.

## Server-room audio

A quiet compressor/fan hum starts with the application. B, L, F and H play a
short button tone once per press (holding a key does not repeat it).
B also starts/stops a sweeping alarm siren, synchronized with alarm lighting.
The hum continues underneath the siren. Sound fades in and the alarm fades out
to avoid abrupt transitions.

Audio is synthesized at runtime; no sound files or extra system installation
are needed. miniaudio 0.11.23 is bundled in external/miniaudio with its license.
If the output device cannot start, the application prints a warning and keeps
running silently.
