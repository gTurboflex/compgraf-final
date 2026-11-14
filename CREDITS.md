# Credits

## Libraries

GLFW and GLM versions and download hashes are defined in
[`cmake/Dependencies.cmake`](cmake/Dependencies.cmake). Upstream notices are
retained in the downloaded sources or bundled files.

| Library | Author and source | License | Use |
| --- | --- | --- | --- |
| GLFW 3.5.1 | Marcus Geelnard, Camilla Löwy and contributors; [source](https://github.com/glfw/glfw/tree/3.5.1) | [zlib/libpng](https://github.com/glfw/glfw/blob/3.5.1/LICENSE.md) | Window, OpenGL context and input |
| GLM 1.0.3 | G-Truc Creation and contributors; [source](https://github.com/g-truc/glm/tree/1.0.3) | [MIT option](https://github.com/g-truc/glm/blob/1.0.3/copying.txt) | Vectors, matrices and transformations |
| GLAD 2 | David Herberth and contributors; [source](https://github.com/Dav1dde/glad/tree/glad2) | [MIT for GLAD, Apache 2.0 for Khronos specifications, and notices for Khronos headers](https://github.com/Dav1dde/glad/blob/glad2/LICENSE) | Pregenerated OpenGL 3.3 loader from GLFW's `deps/glad/gl.h`, compiled by `cmake/glad.c` |
| miniaudio 0.11.23 | David Reid; [source](https://github.com/mackron/miniaudio/tree/0.11.23) | MIT No Attribution option; [bundled license](external/miniaudio/LICENSE) | Audio-device output |

The generated GLAD header includes Khronos platform definitions. No separate
GLAD generator or extra runtime loader installation is needed.

## Project assets

| Asset | Origin and contribution | Source and licensing status |
| --- | --- | --- |
| `floor.pgm`, `metal.pgm`, `panel.pgm`, `wall.pgm` | Procedural 128x128 grayscale maps, added with Member 5's texturing work. Generation assistance is described in README. | [Original texture commit](https://github.com/gTurboflex/compgraf-final/commit/8bec103). Created for this project; no third-party image source or asset license. |
| Room, racks, doors, fixtures, cooling unit, monitor, wall panel and cable trays | Project geometry constructed from a shared cube using C++ and GLM. | `src/Geometry.cpp`, `src/Scene.cpp`, `src/Rack.cpp`. No imported models. |
| Fan hum, button tones and alarm siren | Synthesized at runtime by the project's AudioSynth module. | `src/AudioSynth.cpp`. No third-party recordings. |
| Bitmap HUD glyphs | Character patterns stored in the project's UI implementation. | `src/UI.cpp`. No external font file is loaded by the application. |

These project assets are separate from the third-party libraries above.
This credits file does not assign a new license to the team's work.

## Technical references

- [GLFW quick-start guide](https://www.glfw.org/docs/latest/quick_guide.html):
  reference for window/context setup and use of GLFW's generated GLAD header.
  Upstream examples carry the GLFW license listed above.
- Computer Graphics Fundamentals lecture material: *Computer Viewing*,
  *Hierarchical Modeling*, *Shading*, *Texture Mapping*, *2D Texture Mapping*,
  *Pixels and Buffers*, *Solid Models* and *Procedural Methods*. These supply
  the background for MVP matrices, parent-child transforms, normals, depth
  testing, UVs and repeated geometry. Source: course materials supplied by
  the instructor. Lecture PDFs and figures are not redistributed here.

The lighting, LED and fog GLSL source is in `shaders/`. There are no imported
third-party shader files. Any tutorial code or external asset added later
needs its author, source and license recorded here.
