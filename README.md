# FlowForge

FlowForge is a real-time C++ traffic + crowd simulation engine built for hackathon demos where performance and clarity matter.

![City Rush Hour Placeholder](<img width="2796" height="1538" alt="image" src="https://github.com/user-attachments/assets/51887c05-8a6b-464a-918e-95c1b10893c6" />

![Evacuation Placeholder](<img width="2454" height="1310" alt="image" src="https://github.com/user-attachments/assets/07acc8f3-abce-4986-914e-e03849b91b09" />

![Interactive Editing Placeholder](<img width="2574" height="1422" alt="image" src="https://github.com/user-attachments/assets/46f43a44-8090-487e-81d2-2a8cec2593e7" />


## Why this is impressive
- Deterministic **fixed-step** simulation (60 Hz), seedable RNG
- **SoA agent storage** for cache-friendly updates at scale
- Supports **5k-20k agents** (pedestrians + vehicles)
- Dual pathfinding:
  - **Flow fields** for high-throughput crowd routing
  - **A*** debug mode for single-agent path validation
- Spatial partitioning (uniform grid) for neighbor queries
- Traffic-light intersections + queueing behavior
- Live obstacle editing with instant rerouting
- Demo-ready overlays: heatmap, flow vectors, collision debug, IDs
- Benchmarks and tests included

## Features
### Simulation Core
- 60 Hz fixed timestep
- 2 agent types:
  - `Pedestrian`: faster turning, lower top speed
  - `Vehicle`: slower turning, higher speed, red-light stop behavior
- Collision avoidance: local separation + steering
- Intersections with timed traffic lights
- Runtime obstacle painting (`E` toggles edit mode)

### Demo Modes
- `1` City Rush Hour
- `2` Evacuation Mode
- `3` Interactive Editing

### HUD + Overlays
- HUD: FPS, active agents, avg speed, congestion, evac time
- Heatmap (`H`), flow vectors (`F`), collision debug (`C`), IDs (`I`)

## Architecture
```text
+---------------------------------------------------------+
|                        FlowForge                        |
+----------------------+----------------------------------+
|       UI/Input       | Keyboard/mouse, demo controls   |
+----------------------+----------------------------------+
|      Renderer        | SDL2 draw, overlays, HUD/font   |
+----------------------+----------------------------------+
|       Sim Core       | Fixed-step update loop          |
|  - AgentSoA          | SoA state arrays                |
|  - SpatialGrid       | Neighbor queries                |
|  - FlowFieldSystem   | Crowd pathing                   |
|  - AStar             | Debug single-agent pathing      |
|  - Grid + Lights     | Roads, obstacles, intersections |
|  - Metrics           | Congestion/hotspots/evac time   |
+----------------------+----------------------------------+
|       Utilities      | RNG, math types, profiler hooks |
+----------------------+----------------------------------+
```

## Repository Layout
```text
FlowForge/
├── CMakeLists.txt
├── .clang-format
├── include/flowforge/
│   ├── sim/
│   ├── render/
│   ├── ui/
│   └── util/
├── src/
│   ├── sim/
│   ├── render/
│   ├── ui/
│   └── util/
├── tests/
├── bench/
├── assets/
├── CONTRIBUTING.md
└── LICENSE
```

## Build
### Requirements
- CMake 3.16+
- C++20 compiler (Clang/GCC/MSVC)
- SDL2

### macOS/Linux
```bash
cmake -S . -B build -DFLOWFORGE_BUILD_TESTS=ON -DFLOWFORGE_BUILD_BENCH=ON
cmake --build build -j
./build/flowforge
```

### Windows (Visual Studio)
```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -DFLOWFORGE_BUILD_TESTS=ON -DFLOWFORGE_BUILD_BENCH=ON
cmake --build build --config Release
.\build\Release\flowforge.exe
```

## Run Tests
```bash
ctest --test-dir build --output-on-failure
# or
./build/flowforge_tests
```

## Run Benchmark
```bash
./build/flowforge_bench
```

## Performance Table
Populate this table with your machine output from `flowforge_bench`.

| Agents | ms/frame | Neighbor Query ms |
|---:|---:|---:|
| 1,000 | 1.888 | 0.542 |
| 5,000 | 7.900 | 3.879 |
| 10,000 | 17.783 | 9.664 |
| 20,000 | 42.738 | 24.423 |

Target: ~16.6 ms/frame for 10k agents on a laptop (60 FPS goal). Current run is near-target on this machine.

## Controls
- `Space`: pause/resume
- `N`: step one frame
- `+` / `-`: simulation speed multiplier
- `WASD` / arrows: pan
- `Mouse wheel`: zoom
- `P`: toggle path mode (Flow/A*)
- `H`: heatmap overlay
- `F`: flow vectors overlay
- `C`: collision debug overlay
- `I`: agent IDs overlay (subset)
- `E`: obstacle edit mode toggle
- `LMB drag` in edit mode: draw obstacle wall
- `RMB drag` in edit mode: erase obstacle wall
- `1/2/3`: switch demo mode
- `Esc`: quit

## 60-Second Judge Demo Script
1. Start in **City Rush Hour** (`1`), point at light-controlled intersections and congestion growth.
2. Press `H` to show heatmap and call out top bottlenecks.
3. Press `P` to switch to A* debug mode and mention dual-path system.
4. Switch to **Evacuation Mode** (`2`) and show evac timer behavior as agents clear exits.
5. Switch to **Interactive Editing** (`3`), press `E`, draw a wall with mouse drag, show instant rerouting.
6. Mention benchmark executable and test suite.

## Profiling Hooks
- Scoped timer built in (`ScopedTimer`)
- Optional Tracy hooks: `-DFLOWFORGE_ENABLE_TRACY=ON`

## License
MIT
