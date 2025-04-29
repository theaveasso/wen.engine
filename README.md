## Minimal Engine Frame structure


[Solid Objects] → [Lighting] → [Skybox] → [Transparent Objects] → [PostFX] → [UI]
- [x] Frame Begin

- [ ] Update Cameras / Scene

- [ ] Shadow Pass
    -[ ] Render depth maps from lights

- [ ] Geometry Pass (G-Buffer Fill)
    -[ ] Fill position, normal, albedo, material buffers

-[ ] Lighting Pass
    -[ ] Use G-Buffers to compute scene lighting
    -[ ] Apply shadows
    -[ ] Apply emissive

-[ ] Skybox Pass
    -[ ] Draw background

-[ ] Transparent Pass
    -[ ] Draw transparent objects sorted back-to-front

-[ ] Post-Processing
    -[ ] Bloom
    -[ ] Color grading
    -[ ] Tone mapping
    -[ ] Anti-aliasing

- [x] Frame End
