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
`./build/OpenGLApp --smoke-test` renders for about 1.2 seconds and checks that
the FPS counter updates. Shader and OpenGL errors cause a nonzero exit.

## Build (Windows, PowerShell)

Prerequisites: Git, CMake 3.24 or newer, and Visual Studio 2022 or newer with
the **Desktop development with C++** workload (including MSVC and a Windows
SDK). Install missing Git/CMake tools with:

```powershell
winget install --id Git.Git -e
winget install --id Kitware.CMake -e
```

Use Visual Studio Installer to add the C++ workload if needed, then open a new
PowerShell terminal so installed tools are on `PATH`.

From the repository root, install GLFW and GLM using
[vcpkg](https://github.com/microsoft/vcpkg). Run this setup once per checkout;
`.deps/` is ignored by Git. GLAD is already bundled in `external/glad`.

```powershell
git clone --depth 1 https://github.com/microsoft/vcpkg.git .deps/vcpkg
.\.deps\vcpkg\bootstrap-vcpkg.bat -disableMetrics
.\.deps\vcpkg\vcpkg.exe install glfw3:x64-windows glm:x64-windows
```

Configure an x64 build with the
[vcpkg CMake toolchain](https://learn.microsoft.com/en-us/vcpkg/users/buildsystems/cmake-integration),
then build and run:

```powershell
$vcpkgToolchain = (Resolve-Path .\.deps\vcpkg\scripts\buildsystems\vcpkg.cmake).Path
cmake --fresh -S . -B build -A x64 "-DCMAKE_TOOLCHAIN_FILE=$vcpkgToolchain" -DVCPKG_TARGET_TRIPLET=x64-windows
cmake --build build --config Release --parallel
.\build\Release\OpenGLApp.exe
```

`--fresh` clears old CMake configuration (including missing-package results)
when switching to vcpkg. CMake selects the installed Visual Studio generator;
both it and vcpkg must target x64. The toolchain makes GLFW and GLM discoverable
and copies the required GLFW DLL beside the executable. Keep the source
checkout available for shaders and textures.

For later builds, only the `cmake --build` command is needed. Verify with:

```powershell
ctest --test-dir build -C Release --output-on-failure
.\build\Release\OpenGLApp.exe --smoke-test
```

If CMake still reports `glfw3` or `glm` missing, rerun the configure command
with the toolchain argument. If `powershell.exe` is not found during vcpkg
setup, restore the standard Windows tool directories for the current terminal
and retry:

```powershell
$env:PATH = "$env:SystemRoot\System32\WindowsPowerShell\v1.0;$env:SystemRoot\System32;$env:PATH"
```

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

