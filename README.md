# Waterland

A standalone Linux OpenGL water surface in C++20. Uses four animated Gerstner waves, analytic wave normals, directional lighting, Fresnel-style reflections, and a free-fly camera. The sky is currently a clear-color backdrop; real reflected scenery, foam, refraction, and infinite ocean tiles are future milestones.

## Build

Install a C++ compiler, CMake, GLFW development headers, GLM, and OpenGL development headers. For example, on Arch/EndeavourOS:

```sh
sudo pacman -S --needed base-devel cmake glfw-x11 glm mesa
cmake -S . -B build
cmake --build build -j
cd build && ./water
```

On Debian/Devuan (with the appropriate GL driver installed):

```sh
sudo apt install build-essential cmake libglfw3-dev libglm-dev libgl-dev
cmake -S . -B build
cmake --build build -j
cd build && ./water
```

## Controls

WASD move; mouse look; Space rise; Left Ctrl descend; Left Shift move faster; Esc exit.

## Notes

Requires an OpenGL 4.1+ context (uses GLSL 410). Designed to be extended into a general-purpose renderer.
