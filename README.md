# Minecraft-Inspired Voxel Sandbox   

A small first-person, block-based 3D sandbox built with **C++** and **raylib** as a first-semester programming project at **Madhav Institute of Technology and Science (MITS), Gwalior**.

[![Language: C++](https://img.shields.io/badge/Language-C%2B%2B-00599C?logo=cplusplus&logoColor=white)](https://isocpp.org/)
[![Graphics: raylib](https://img.shields.io/badge/Graphics-raylib-000000)](https://www.raylib.com/)
[![Project status](https://img.shields.io/badge/Status-Learning%20Prototype-orange)](#current-limitations)

> This is an educational Minecraft-inspired prototype, not a complete recreation of Minecraft. You should know that

## Overview

This project explores the fundamentals of real-time 3D game programming. It creates a small voxel world from a three-dimensional grid, renders textured blocks, and lets the player move through the environment, jump, remove blocks, and place new ones.

The world is stored in a fixed **30 × 20 × 30** grid. Its initial terrain contains layers of stone, dirt, and grass. A first-person camera, ray-based block selection, basic movement physics, exposed-block rendering, background music, and a simple on-screen HUD bring the prototype together.

**Author:** Divyansh Rajput
**Academic context:** First semester · MITS, Gwalior  
**Language:** C++  
**Graphics/audio library:** raylib

## Features

- **Voxel world:** Represents blocks in a fixed 3D array with `AIR`, `GRASS`, `DIRT`, and `STONE` block types.
- **First-person controls:** Looks around and moves through the world using raylib's first-person camera controls.
- **Block interaction:** Uses a ray from the camera to target blocks within a six-unit reach; left-click removes a targeted block and right-click places the selected block beside it.
- **Block selection:** Selects grass, dirt, or stone with keyboard shortcuts.
- **Basic player physics:** Includes directional movement, gravity, jumping, and simple solid-block collision checks.
- **Textured rendering:** Applies separate grass, dirt, and stone textures to a reusable cube model.
- **Exposed-block culling:** Skips rendering occupied cells that are fully surrounded by solid blocks, except at the world boundary.
- **Audio:** Streams looping background music with adjustable volume.
- **HUD:** Displays the current block selection, a center-screen crosshair, and the FPS counter.
- **Resource cleanup:** Unloads the model, textures, music stream, audio device, and window during shutdown.

## Tech Stack

| Technology | Role |
| --- | --- |
| **C++** | Game logic, world data, movement, and interaction |
| **raylib** | Windowing, input, camera, rendering, textures, and audio |
| **raymath** | Vector operations used by the camera, raycasting, and movement code |
| **rlgl** | Rendering utility used to adjust the targeted block's wireframe thickness |
| **Dev-C++** | Original development environment |

## Controls

| Input | Action |
| --- | --- |
| `W` / `A` / `S` / `D` | Move |
| Mouse | Look around |
| `Space` | Jump when grounded |
| `Left Mouse Button` | Break the targeted block |
| `Right Mouse Button` | Place the selected block beside the targeted block |
| `Z` | Select grass |
| `X` | Select dirt |
| `C` | Select stone |
| `Esc` | Close the game window |

## How It Works

The prototype follows a single game-loop workflow:

1. **Initialize:** Create the window and audio device, load textures and music, and create one reusable cube model.
2. **Generate the world:** Fill the lower six grid layers with stone, dirt, and grass.
3. **Read input:** Update the first-person camera, movement direction, jump input, and selected block type.
4. **Find a target:** Step a ray outward from the camera and check the voxel grid for a solid block within reach.
5. **Modify the world:** Remove the hit block or place the selected block in the adjacent cell, subject to the current coordinate and distance checks.
6. **Update movement:** Apply gravity and velocity using the frame time, then perform simple collision checks.
7. **Render:** Draw exposed solid blocks, outline the targeted block, and render the crosshair, FPS counter, and selected-block label.
8. **Clean up:** Release graphics and audio resources when the window closes.

### Simplified architecture

```text
Keyboard / Mouse
       |
       v
Camera + Player Movement -----> Basic Physics / Collision
       |                                  |
       v                                  v
Camera Raycast -----------------> 3D Voxel Grid
       |                         (AIR / GRASS / DIRT / STONE)
       |                                  |
       +------ Break / Place --------------+
                                          |
                                          v
                         Exposed-Block Rendering + HUD
                                          |
                                          v
                                  Displayed Frame
```

## Requirements

- A C++ compiler supported by your raylib installation.
- **raylib** configured for that compiler and platform.
- The runtime assets listed below.

### Required runtime assets

The source loads these files by name:

```text
grass.png
dirt.png
stone.png
bgm.mp3
```

Place the files in the program's **working directory** when running the game, or update the paths in `main.cpp`. The source file alone does not contain these assets. Make sure you have permission to redistribute any textures or music you add to the repository.

## Build and Run

### Option 1: Dev-C++ (the original environment)

1. Install or configure a version of **raylib compatible with the C++ compiler bundled with your Dev-C++ installation**. raylib must be configured for the same compiler and architecture as the project.
2. Create or open a C++ project and add `main.cpp`.
3. Configure the compiler's include directory and the linker's library directory for your raylib installation.
4. Add the linker libraries required by your raylib build. On a typical Windows/MinGW setup these commonly include `raylib`, `opengl32`, `gdi32`, and `winmm`; follow the instructions for your specific raylib package if they differ.
5. Put `grass.png`, `dirt.png`, `stone.png`, and `bgm.mp3` in the program's working directory.
6. Build and run the project.

### Option 2: Another IDE or command line

Use a C++ toolchain with raylib installed and configured. Create a project for `main.cpp`, link the raylib libraries required by your operating system, and ensure the runtime assets are available from the process's working directory. Exact compiler and linker commands depend on how raylib was installed, so use the build instructions for that installation rather than copying flags from an unrelated setup.

## Project Structure

The current source provided for this repository is `main.cpp`. The following is a suggested repository layout; add the asset files when you have them available:

```text
minecraft-voxel-sandbox/
├── main.cpp       # World, player, raycasting, input, rendering, and cleanup
├── grass.png      # Grass block texture (required at runtime)
├── dirt.png       # Dirt block texture (required at runtime)
├── stone.png      # Stone block texture (required at runtime)
├── bgm.mp3        # Looping background music (required at runtime)
└── README.md      # Project documentation
```

## Engineering Notes

### Challenges addressed

- **Representing a 3D world:** Used a fixed three-dimensional array to map integer grid coordinates to block types, keeping block lookup and world updates straightforward.
- **Targeting blocks in 3D:** Implemented incremental ray stepping from the camera to find a solid block in the direction the player is looking.
- **Reducing unnecessary rendering:** Added a neighbor check so fully enclosed blocks are not drawn, while boundary blocks remain visible.
- **Avoiding repeated model creation:** Reused one cube model and changed its diffuse texture based on the block being rendered.
- **Coordinating movement with frame time:** Used the frame delta for position updates and gravity so movement is not directly tied to a fixed number of updates per second.
- **Managing resources:** Added explicit cleanup for the loaded model, textures, music stream, audio device, and window.

These are the current implementation choices, not claims of a production-grade engine. Collision handling, raycasting, asset validation, and world management can be improved further.

## Skills Demonstrated

- C++ fundamentals, functions, structures, enumerations, and multidimensional arrays
- 3D coordinate systems and vector mathematics
- Real-time game loops and frame-time-based updates
- Keyboard and mouse input handling
- First-person camera control
- Basic gravity, jumping, and collision detection
- Ray-based object selection
- Textured 3D rendering and visibility checks
- Audio playback and resource lifecycle management
- Debugging and documenting a small interactive application

## Screenshots and Demo

Screenshots and a gameplay demo will be added after the visuals are captured.

<!-- Replace these placeholders with real screenshots or a recorded demo when available. -->

- **Gameplay screenshot:** Coming soon. Add a world-view image here when captured.
- **Block interaction screenshot:** Coming soon. Add an image showing block placement or removal here.
- **Demo video:** Coming soon.

## Current Limitations

- The world is fixed-size and is not saved between runs.
- Terrain generation is a hard-coded layer setup rather than procedural generation.
- Collision detection is intentionally basic and may not handle every edge case.
- Block targeting uses incremental ray steps rather than a grid traversal algorithm such as 3D DDA.
- The selected block is a single placement type, not a full inventory or hotbar system.
- The source does not currently validate whether texture and audio assets loaded successfully.
- The repository needs the four runtime assets to run as intended; they were not part of the supplied `main.cpp` file.

## Future Improvements

- Add a **3D DDA voxel traversal** algorithm for more precise and efficient block targeting.
- Improve player collision with a consistent player bounding volume and separate horizontal/vertical collision resolution.
- Add world save/load functionality and configurable world generation.
- Replace the single selected-block label with a proper hotbar and inventory.
- Add asset-loading checks and clear error messages when required files are missing.
- Organize the game into focused modules, such as `World`, `Player`, `Renderer`, and `Assets`, as the project grows.
- Add build instructions or project files for reproducible builds across supported platforms.
- Capture screenshots and a short gameplay video for the portfolio.
- Profile rendering and consider chunk-based meshes if world size increases.

## Resume-Ready Project Summary

- Developed a **C++ first-person voxel sandbox** using raylib, with a 3D grid-based world and interactive block placement and removal.
- Implemented camera raycasting, basic player movement physics, jumping, gravity, and collision checks to support real-time interaction.
- Added exposed-block culling and a reusable cube model to avoid drawing fully enclosed blocks and repeatedly creating identical models.
- Integrated texture mapping, looping background audio, an FPS counter, and on-screen interaction feedback.

## Contributing

Contributions and suggestions are welcome. To propose a change:

1. Fork the repository.
2. Create a focused branch for your change.
3. Keep changes small and explain the problem they solve.
4. Test the project with a compatible raylib setup and the required runtime assets.
5. Open a pull request describing the change and any relevant testing.

For bug reports, include your operating system, compiler, raylib setup, steps to reproduce, and any relevant error messages.

## License

No license has been selected for this repository yet. Until a license is added, others do not automatically have permission to copy, modify, or redistribute the code. 

---

**Built by Divyansh Rajput · First-semester project · MITS, Gwalior**
