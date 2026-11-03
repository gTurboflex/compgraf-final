# Server Room final project

## Project context

- Course: Computer Graphics Fundamentals. Topic 27: Server Room, difficulty one star.
- Team: Ергун, Даниал, Нурадилет, Бекасыл, Даурен.
- Repository: https://github.com/gTurboflex/compgraf-final
- Read `docs/TEAM_PLAN_RU.md` before implementation. It records the requirements, proposed ownership, integration order, and submission checklist.
- Source documents reviewed on 2026-10-28: `Final project requirements.pdf` (12 pages) and `Final project topics.pdf` (11 pages, topic 27 on page 9), supplied by the user. The course report template and defense date have not been supplied.
- This is a proposed work plan. As of this review, the application contains only the GLFW/GLAD OpenGL window in `src/main.cpp`. Do not describe planned features as implemented.

## Requirements to preserve

- C++ and OpenGL 3.3+ core profile; VAO/VBO/EBO; own vertex and fragment shaders in separate `.vert` / `.frag` files; no fixed-function rendering or game engine.
- Meet every R1-R8: five distinct scene objects, model/view/projection transforms, a real parent-child hierarchy, per-fragment Phong or Blinn-Phong lighting with ambient/diffuse/specular and at least two lights including one dynamic or toggleable light, textures with UVs and mipmaps or an allowed equivalent, keyboard and mouse camera plus two scene interactions, stable 30+ FPS at 1280x720 on the TA machine with visible FPS, and a reproducible course-compatible build documented in README.
- Aim for the rubric's higher targets: 60+ FPS, three meaningful scene interactions, on-screen help, modular code with files around 500 lines or less, and two working stretch goals.
- Topic Core: repeated racks, emissive LED quads with different shader-driven blink timings, cold blue ceiling lights, walking camera, and a key switching the room to flashing red overheat alarm mode.
- Proposed hierarchy: rack -> hinged door and server units -> LED quads. Proposed scene actions: `1` alarm, `E` nearest rack door, `L` ceiling lights.
- Complete Core and R1-R8 before stretch work. Proposed stretch goals: cable trays plus textured floor tiles as ONE combined goal, and cold-air fog as the second. A live FPS graph on a monitor is optional later work.
- New libraries not used in course assignments must be included in the repository and build without extra installation on the TA machine. Confirm the assignment setup before choosing dependencies. Current CMake uses macOS frameworks; TA platform compatibility is still to be verified.
- Preserve third-party licenses and record author, link, license, and use in CREDITS. Cite reused tutorial code in code and report. Disclose actual AI use in README and report. Do not invent research citations, measurements, or contributions.
- Every member contributes real code and regular commits under their own identity, writes about their implementation, presents part of the defense, and understands the full application. Do not manufacture commit histories.
- Deliverables: repository with TA access and a `v1.0` release; report using the course template; 8-12 slides / 7-10 minutes; 3-6 minute 1080p demo showing R1-R7 and claimed stretch goals, with visible FPS throughout and narration or captions. Submit to Moodle three days before defense by 23:59 Asia/Almaty.
- Report full-score target: 15+ credible sources from Q1-Q2 journals, compared in the literature review; technical equations and diagrams; feature screenshots; FPS table with hardware; references and specific AI Use note. Every member contributes three distinct journal sources as proposed in the team plan.

## Proposed module ownership

- Ергун: application, Shader/Mesh base, common types, build, integration, README and release coordination.
- Даниал: room, racks, doors, scene hierarchy, object layout.
- Нурадилет: Camera, input actions, on-screen help, FPS, reproducible performance checks.
- Бекасыл: lighting, GLSL shaders, LED animation, alarm, and fog after Core.
- Даурен: textures, UV/material integration, cable trays and floor tiles; optional monitor FPS graph; assembling report and video with everyone's contributions.
- Shared files such as `main.cpp`, `CMakeLists.txt`, common headers, and shared shaders require coordination through the file owner. See the plan for proposed paths and interfaces.
