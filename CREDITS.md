# Credits
The project already includes GLAD and uses GLFW. This contribution also uses
GLM (https://github.com/g-truc/glm, MIT license).
All demonstration geometry is generated from primitives; no external assets or
copied shader snippets were added.
AI use: OpenAI Codex assisted with the Member 3 implementation and documentation.
Team members should review the code and explain the equations during defense.

## Member 5 assets
The floor, painted wall, brushed metal and perforated panel images in
assets/textures/ were generated specifically for this project by OpenAI Codex
using the accompanying generate.py (seed 27). They use no third-party artwork,
samples or source images. The texture loader and fog shader contributions were
also generated for this project. No external asset license is required.

## Member 4

OpenAI Codex assisted with Camera, Input, UI, PerformanceStats, the hinged-door
integration, tests, portable builds, and documentation. The HUD font is an
embedded 5-by-7 bitmap defined in UI.cpp. No downloaded font is used.

Bundled dependencies:
- GLFW 3.4: https://github.com/glfw/glfw, zlib/libpng license;
  commit 7b6aead9fb88b3623e3b3725ebb42670cbe4c579.
- GLM 1.0.1: https://github.com/g-truc/glm, MIT license option;
  commit 0af55ccecd98d4e5a8d1fad7de25ba429d60e863.

Their license files and upstream records are in external/glfw and external/glm.
