# Topic 27 — Server Room

## Dependencies

CMake downloads and builds the project's libraries automatically.
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
installed globally.

A clean PC still needs **CMake 3.24+**, a **C/C++ compiler and platform SDK**,
and graphics drivers supporting **OpenGL 3.3**. CMake manages project libraries;
it does not install the compiler, operating-system SDK, or graphics drivers.

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

## Project layout

```text
cmake/          Dependency versions, download rules, and the small GLAD adapter
src/            Application C++ source files
include/        Project headers (smoke-test helpers in include/tests/)
shaders/        GLSL shader programs
assets/         Runtime textures
tests/          Automated tests
docs/           Reports and integration notes
.vscode/        Shared editor settings
CMakeLists.txt  Application and test build targets
build/          Generated files only; ignored by Git
  _deps/        Downloaded library sources and their build files
```

Deleting `build/` resets all build output and cached downloads; CMake recreates them on the next configure.
Keep new generated output under `build/` rather than adding folders at the root.

## Team responsibilities

### Member 1 — Application setup and rendering foundation
- Configure CMake, dependencies, and build instructions.
- Initialize GLFW, GLAD, the OpenGL context, and rendering settings.
- Develop the Shader class for loading, compiling, linking, and setting uniforms.
- Develop the Mesh class for managing VAO/VBO/EBO buffers and drawing geometry.
- Organize application setup, the render loop, resource cleanup, and module integration.
- Maintain README build instructions and prepare the final release.

### Member 2 — Models, scene layout, and hierarchy
- Build the room, reusable server racks, server units, and supporting equipment.
- Arrange rack copies and other objects into a coherent server-room scene.
- Define geometry, normals, and UV coordinates in coordination with Member 5.
- Implement model transformations and a parent–child hierarchy.
- Create hinged rack-door animation and cable-tray geometry.
- Document scene construction and transformations; assemble the team’s presentation slides.

### Member 3 — Lighting, shaders, and server effects
- Write the GLSL vertex and fragment shaders for lighting and LEDs.
- Implement per-fragment Blinn–Phong shading with ambient, diffuse, and specular terms.
- Add multiple cold-blue ceiling lights and support toggling them.
- Implement emissive server LEDs with independent randomized blink timing.
- Implement the overheat alarm mode with flashing-red lighting.
- Document the lighting equations, shader logic, and alarm behavior.

### Member 4 — Camera, interaction, UI, and performance
- Implement keyboard-controlled walking and mouse-look camera movement.
- Keep movement independent of frame rate and update projection when the window resizes.
- Connect controls for alarm mode, ceiling lights, fog, and rack-door interaction.
- Display controls help, FPS, and frame-time information.
- Test navigation and interactions; measure performance at 1280×720.
- Prepare the testing section and FPS table; assemble the team’s demo video.

### Member 5 — Textures, materials, and atmosphere
- Develop texture loading, filtering, wrapping, and mipmap generation.
- Create and apply materials for the floor, walls, racks, and server panels.
- Coordinate UV mapping with Member 2 and texture sampling with Member 3.
- Implement textured floor tiles and cold-air fog.
- Maintain a consistent visual style and record asset sources and licenses in CREDITS.md.
- Document textures and atmosphere; assemble the report from everyone’s contributions.

## Current Features:

### Controls, camera, and FPS

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

### Visuals

- Blinn–Phong lighting with ambient, diffuse, and specular terms
- Multiple cold-blue ceiling lights with toggle support
- Emissive server LEDs with randomized independent blink timing
- Overheat alarm mode with flashing-red lighting
- Procedural and loaded textures for floor tiles, walls, racks, and server panels
- Cold-air volumetric fog with toggle support
- On-screen HUD showing FPS, frame time, alarm/light/fog status, and help panel

### Server-room audio

A quiet compressor/fan hum starts with the application. B, L, F and H play a
short button tone once per press (holding a key does not repeat it).
B also starts/stops a sweeping alarm siren, synchronized with alarm lighting.
The hum continues underneath the siren. Sound fades in and the alarm fades out
to avoid abrupt transitions.

Audio is synthesized at runtime; no sound files or extra system installation
are needed. miniaudio 0.11.23 is bundled in external/miniaudio with its license.
If the output device cannot start, the application prints a warning and keeps
running silently. See [audio details](docs/audio.md).

## AI use

OpenAI Codex assisted with project planning and generated the procedural
textures credited in CREDITS.md. On 11 November 2026, Codex repaired duplicated
code from the latest merge and integrated the Rack model with CMake, the
indexed Mesh renderer, and existing materials. Codex ran the Windows Release
build, CTest, and OpenGL smoke test, and inspected a framebuffer capture.
These automated checks do not replace team review or testing on the TA machine.
The report-ready disclosure is in [docs/report/ai-use.md](docs/report/ai-use.md).