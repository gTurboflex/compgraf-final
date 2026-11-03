# Team video and Member 4 demonstration

The final team video must be 3–6 minutes and 1080p. Record the running application. Keep the FPS counter visible. Use the team's actual build and measured results. This plan is a recording guide; it is not a claim that a final video has already been made.

## Before recording

1. Build and run the program using the README commands.
2. Check the real keyboard and mouse controls in the application window.
3. Close private windows and notifications. Hide the recorder's mouse pointer when possible. Keep the application HUD readable.
4. Make a short test recording. Watch it and check the picture, sound, and FPS text.
5. Use the results file from the team's 1280×720 test for the performance section. A 1080p recording does not change the stated resolution of that separate test.

## Suggested team sequence

| Time | Content |
|---|---|
| 0:00–0:20 | Show the topic, team members, and normal blue server-room view. |
| 0:20–1:00 | Explain the project structure and scene objects. Each owner describes the code that is actually in the final build. |
| 1:00–2:20 | Show the Member 4 controls using the sequence below. |
| 2:20–3:10 | Show the lighting, LED timing, materials, and fog. Explain the work of Members 3 and 5. |
| 3:10–3:40 | Show the measured FPS table with CPU, GPU, resolution, test duration, and VSync state. Mention any remaining performance issue and its module owner. |
| 3:40–4:00 | Return to a normal blue view, pause, then press B to end on the flashing red alarm mode. |

These times can change, but the complete video must stay within 3–6 minutes. Leave each important visual result on screen long enough to see it.

## Member 4 segment: about 60–90 seconds

Start with the camera reset using `R`, normal alarm state, and ceiling light on. The HUD shows these states. Click the window or press `Tab` if mouse-look is released.

| Step | Action | Short explanation |
|---|---|---|
| 1 | Show the help panel with H if needed. | “My part handles the camera, controls, help, and FPS.” |
| 2 | Walk with W, A, S, and D. Move the mouse slowly. | “WASD moves the camera. The mouse changes the view direction.” |
| 3 | Walk diagonally for a moment. | “Movement uses delta time. Diagonal movement is normalized.” |
| 4 | Approach a rack until the bottom prompt names its door. Press E, pause, then E again. | “E opens or closes the nearest door when the camera is close enough.” |
| 5 | Press B, pause, then B again. | “B changes alarm mode.” |
| 6 | Press L, pause, then L again. | “L changes the ceiling light.” |
| 7 | Press H to hide the full help panel. | “The FPS counter stays visible when help is hidden.” |
| 8 | Press Tab to release the mouse, resize the window, then click to resume. | “The projection uses the new aspect ratio after resizing.” |
| 9 | Press R. | “R resets the camera.” |

F controls the existing fog effect. Show it in the Member 5 section, or briefly here if the team needs to explain the shared input mapping. Do not use A for alarm: A now belongs to walking.

Keep the FPS panel on screen throughout the segment. Move the mouse slowly enough for viewers to follow the camera. A door outside the allowed range should not open; show the proximity prompt rather than repeatedly pressing E from far away.

## Performance section

Use the recorded results from the final build. State the tested resolution of 1280×720, the CPU and GPU, and whether VSync was enabled. Explain that the ordinary interactive run and the uncapped benchmark can show different FPS values.

Do not read an FPS value from one frame and call it an average. Use the benchmark's mean FPS and frame-time data. If a test has not been run, mark it as pending instead of adding a number.

## Final review and assembly

- Check that the final file is 1080p and 3–6 minutes long.
- Check that all three interactions are visible: alarm, nearby rack door, and ceiling light.
- Check that WASD, mouse-look, resize handling, and the visible FPS counter are shown.
- Check the team credits and each member's contribution description.
- Show only implemented features and actual measurements.
- Keep the required AI-use note in the README and report.
- End with a clear normal blue view changing to red alarm mode.
- Watch the exported file from start to end, then include it in the team's final submission.
