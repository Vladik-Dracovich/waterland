# WaterLand-C

A very early C + OpenGL rendering of watery landscape.
Everything is a work in progress, this is just pulled up for now and I'll slowly work on it as I go along.

Don't download and expect this repo to run, as I need to still fix and work on alot. Anyone is free to take and use as they please from this.

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
