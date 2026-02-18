# WaterLand-C

A minimal **C + OpenGL** water landscape demo that goes up to **Stage 3**:

- **Stage 1**: Gerstner-wave water surface (vertex displacement + normals)
- **Stage 2**: Depth-based absorption + refraction (scene color behind water + depth)
- **Stage 3**: **Planar reflections** via a mirrored render pass into an FBO

This is a clean, self-contained demo project (no engine).

## Dependencies (Linux)
- GLFW
- GLEW
- OpenGL
- `pkg-config`

Debian/Ubuntu:
```bash
sudo apt-get install -y build-essential pkg-config libglfw3-dev libglew-dev
```

Arch:
```bash
sudo pacman -S --needed base-devel pkgconf glfw-x11 glew
```

## Build
```bash
make
```

## Run
```bash
./waterland
```

Controls:
- **WASD**: move
- **Mouse**: look
- **Space / Left Shift**: up / down
- **Esc**: quit

## Notes
- Reflection is rendered by mirroring the camera across the water plane (y = 0).
- Refraction uses a scene color FBO + a depth texture to compute absorption and shoreline shading.
- Water is rendered last, sampling:
  - reflection color
  - refraction color
  - refraction depth

## Screenshot/GIF
Add your own screenshot to `assets/` and embed it here once you run it.
