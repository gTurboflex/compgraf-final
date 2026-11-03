# Lighting and effects — Member 3
All lighting calculations use world coordinates. Vertex positions use M p;
normals use transpose(inverse(mat3(M))) n, then are normalized per fragment.
This preserves perpendicular normals under nonuniform scales. Models must have
nonzero scale on every axis.

For each point light:
L = normalize(lightPosition - worldPosition), V = normalize(eye - worldPosition),
H = normalize(L + V).
The fragment color is:
C = 0.07 * albedo * tint
  + sum(tint * (albedo * max(N·L,0) + 0.4 * max(N·H,0)^48)
        * 3 / (1 + 0.09*d + 0.032*d*d)).
Specular is suppressed on faces facing away from the light.
Two lights sit at (-2,3.5,-1) and (2,3.5,-1). L disables their direct terms;
a small ambient term remains for navigation.

Normal tint is cold blue. Alarm tint is red multiplied by
0.2 + 0.8*(0.5 + 0.5*sin(2*pi*2*time))^3.
Both direct and ambient terms follow the alarm so the room visibly pulses red.
The controls module can change Lighting state without altering shader code.

LEDs are front-facing emissive quads, rendered with depth testing. Emissive here
means independent of incident lighting, not bloom or illumination of nearby
geometry. Their stable IDs seed different phases and rates (0.7–3.7 Hz).
A hash of ID and time bucket chooses each on/off state entirely in GLSL;
the dim state retains 6% brightness. Alarm changes their tint to red.

## Manual acceptance
Run the scene: inspect diffuse shading and highlights on scaled rack panels.
Observe LEDs over several seconds: adjacent LEDs must have different activity.
Press B: blue room changes to pulsing red; press again to restore blue.
Member 4 moved this control from A to B to leave A available for WASD walking.
Press L in either mode: direct ceiling illumination disappears and returns.
Hold either key: it must toggle only once. Resize the window and verify aspect.

The smoke test checks real context creation, shader compilation/linking and GL
errors in all four toggle combinations. It is not a visual inspection or an FPS
benchmark on the TA machine.
