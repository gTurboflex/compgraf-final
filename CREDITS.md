# Credits
Dependencies are downloaded by CMake from pinned upstream archives:

- [GLFW](https://github.com/glfw/glfw), zlib/libpng license.
- [GLM](https://github.com/g-truc/glm), MIT license.
- [GLAD](https://github.com/Dav1dde/glad), using the pregenerated OpenGL 3.3
  header shipped with GLFW. See [GLAD's licensing](https://github.com/Dav1dde/glad/blob/glad2/LICENSE)
  for its generator and Khronos specifications. The header includes the
  Khronos platform definitions and their copyright/permission notice.

All demonstration geometry is generated from primitives; no external assets or
copied shader snippets were added.
AI use: OpenAI Codex assisted with the lighting implementation and documentation.
Team members should review the code and explain the equations during defense.

## Member 5 assets
The floor, painted wall, brushed metal and perforated panel images in
assets/textures/ were generated specifically for this project by OpenAI Codex
using the accompanying generate.py (seed 27). They use no third-party artwork,
samples or source images. The texture loader and fog shader contributions were
also generated for this project. No external asset license is required.
