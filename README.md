# Topic 27 — Server Room
Lighting integration: per-fragment Blinn–Phong, two cold-blue ceiling point lights,
independently blinking emissive LED quads, and flashing red alarm mode.
The primitive room is an integration demonstration for the team's future scene.

## Dependencies

CMake downloads and builds the project's libraries automatically. You do not
need vcpkg, Python, manual header copies, or separate library installation.
The first configure requires internet access; later builds reuse the files
in `build/_deps`. Versions and archive hashes are pinned in
[`cmake/Dependencies.cmake`](cmake/Dependencies.cmake).

| Dependency | How it is provided |
| --- | --- |
| GLFW 3.5.1 | Downloaded from GLFW's official source archive and built as a static library. |
| GLM 1.0.3 | Downloaded from GLM's official source archive and used as headers. |
| GLAD 2, OpenGL 3.3 loader | Pregenerated `deps/glad/gl.h` from the pinned GLFW archive, compiled by `cmake/glad.c`. |
| Khronos/KHR definitions | Embedded in that generated GLAD header; no separate library is needed. |

The loader follows [GLFW's own example setup](https://www.glfw.org/docs/latest/quick_guide.html).
All machines use these same dependency sources, even if other versions are
installed globally. When adding another library, declare its version and hash
in `cmake/Dependencies.cmake` and link its CMake target in `CMakeLists.txt`.

A clean PC still needs **CMake 3.24+**, a **C/C++ compiler and platform SDK**,
and graphics drivers supporting **OpenGL 3.3**. CMake manages project libraries;
it does not install the compiler, operating-system SDK, or graphics drivers.
Windows and macOS instructions follow. Linux also needs the platform development
packages required by GLFW's enabled X11/Wayland backends.

## Build (macOS)
Install Xcode command line tools and CMake if missing:
```sh
xcode-select --install
brew install cmake
```

From the repository root:

```sh
cmake -S . -B build
cmake --build build -j
./build/OpenGLApp
```
Shader paths are resolved from the source directory at configure time. Keep the
source checkout available when running this development build.
A toggles alarm; L toggles ceiling lights; Escape exits. Keys toggle once per press.
`./build/OpenGLApp --smoke-test` renders for about 1.2 seconds and checks that
the FPS counter updates. Shader and OpenGL errors cause a nonzero exit.

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

Keep the source checkout available for shaders and textures. GLFW and GLAD
are linked into the executable, so no GLFW DLL needs to be copied beside it.

If you previously configured with vcpkg, reset the cached toolchain once:

```powershell
cmake --fresh -S . -B build -A x64
```

For later builds, only the `cmake --build` command is needed. Verify with:

```powershell
ctest --test-dir build -C Release --output-on-failure
.\build\Release\OpenGLApp.exe --smoke-test
```

## Project layout

```text
cmake/          Dependency versions, download rules, and the small GLAD adapter
src/            Application C++ source and headers
shaders/        GLSL shader programs
assets/         Runtime textures
tests/          Automated tests
docs/           Reports and integration notes
.vscode/        Shared editor settings
CMakeLists.txt  Application and test build targets
build/          Generated files only; ignored by Git
  _deps/        Downloaded library sources and their build files
```

Library source is not stored in the repository. The previous `external/glad/`
and `.deps/vcpkg/` directories are no longer used. Deleting `build/` resets all
build output and cached downloads; CMake recreates them on the next configure.
Keep new generated output under `build/` rather than adding folders at the root.

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

## Member 4 - FPS calculation

This first stage adds a small FPS counter. The window title shows FPS and
average frame time, updated about twice per second. FPS is the number of frames
divided by their elapsed time. Zero, negative, and non-finite time values are
ignored. Rendering and timing pause while the window is minimized.

The in-scene help panel, benchmark files, walking camera, mouse look, and door
controls are for later stages. Current controls remain A, L, F, and Esc.

Build as above, then run `ctest --test-dir build --output-on-failure` (add
`-C Release` on Windows) to check
the FPS calculation. The tests cover different frame rates, variable frame
times, invalid inputs, and consecutive measurement intervals.

