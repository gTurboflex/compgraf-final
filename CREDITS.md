# Credits
Dependencies are downloaded by CMake from pinned upstream archives:

- [GLFW](https://github.com/glfw/glfw), zlib/libpng license.
- [GLM](https://github.com/g-truc/glm), MIT license.
- [GLAD](https://github.com/Dav1dde/glad), using the pregenerated OpenGL 3.3
  header shipped with GLFW. See [GLAD's licensing](https://github.com/Dav1dde/glad/blob/glad2/LICENSE)
  for its generator and Khronos specifications. The header includes the
  Khronos platform definitions and their copyright/permission notice.
## Audio
- miniaudio 0.11.23 by David Reid:
  https://github.com/mackron/miniaudio/tree/0.11.23
  Bundled header and license in external/miniaudio; used under MIT No Attribution.
- Server hum, button tones and siren are synthesized by AudioSynth.
  No third-party recordings are used.
