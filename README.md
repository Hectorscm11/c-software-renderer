# C Software Renderer

An educational 3D software renderer built from scratch in pure C. This project implements custom linear algebra, 3D transformations, and rasterization pipelines without using any hardware-accelerated graphics APIs (No OpenGL/Vulkan/DirectX). It relies solely on SDL2 to push the final pixel buffer to the screen.

## Features

* **Custom 3D-to-2D Projection:** Implements true perspective projection (handling Z-depth and FOV scaling).
* **3D Transformations:** Real-time object rotation (pitch and yaw) using custom trigonometric matrix math.
* **Line Rasterization:** Custom implementation of Bresenham's line algorithm to draw wireframe meshes.
* **Interactive Object Controls:** Mouse event handling to click and drag, rotating the 3D objects in real-time.


## Prerequisites

To build and run this project, you need a C99 compatible compiler, `make`, and the SDL2 development libraries.

**On Debian/Ubuntu:**
```bash
sudo apt-get update
sudo apt-get install build-essential libsdl2-dev