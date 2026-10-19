# Vision-Guided Plant-Watering Rover

A work-in-progress autonomous rover designed to monitor and water a U-shaped dorm-room garden. The current milestone is a tested C++ perception-to-control simulator that turns processed camera and AprilTag observations into safe navigation actions.

> **Current scope:** simulated perception and C++ control only. Real camera processing, Python/OpenCV integration, Raspberry Pi deployment, and physical hardware are not implemented yet.

## Current status

### Implemented

- C++17 rover controller
- Plant, dock, boundary, and unknown AprilTag classification
- Safety-prioritized decision logic
- Multiple AprilTags in one camera frame
- CSV replay of simulated camera and sensor observations
- Controller and CSV parser tests
- Makefile build commands
- GitHub Actions build and test workflow

### In progress / planned

- Python/OpenCV camera pipeline
- Real AprilTag detection and pose estimation
- Raspberry Pi deployment
- On-device plant-health inference
- Model quantization and compression
- Motor, pump, bumper, and encoder integration
- Physical docking and watering tests

## System architecture

```text
CURRENT
simulated_camera_data.csv
          |
          v
   C++ CSV frame loader
          |
          v
   C++ rover controller
          |
          v
 navigation action + reason

PLANNED
Pi camera -> Python/OpenCV perception -> C++ controller -> motors/pump
```

The perception interface is documented in [`docs/camera-frame-interface.md`](docs/camera-frame-interface.md). The staged physical build and purchasing plan is in [`docs/hardware-bom.md`](docs/hardware-bom.md).

## AprilTag scheme

| IDs | Meaning |
|---|---|
| `1-99` | Plant stations |
| `500` | Home dock |
| `900-999` | Safety boundaries |
| Anything else | Unknown and ignored |

AprilTags are printed visual markers read by a camera. They are not RFID tags and require no radio reader.

## Controller decisions

The controller can return:

- `TURN_LEFT`
- `TURN_RIGHT`
- `DRIVE_STRAIGHT`
- `STOP_AT_TARGET`
- `HOLD_POSITION`
- `EMERGENCY_STOP`
- `SEARCH_FOR_ROUTE`

Safety always overrides navigation:

1. Invalid or stale camera frame
2. Bumper activation
3. Camera-detected obstacle
4. Confident boundary AprilTag
5. Correct target AprilTag
6. Visible route
7. Lost route

If a plant and boundary are visible simultaneously, the boundary wins.

## Build and run

### Requirements

- C++17 compiler
- `make`

On macOS with Xcode Command Line Tools or a typical Linux development environment:

```bash
make
./rover_sim simulated_camera_data.csv 2
```

The final argument is the target AprilTag ID. In this example, the rover is navigating toward Plant 2.

Example output:

```text
Frame 4: TURN_LEFT | target AprilTag is left of center
Frame 7: STOP_AT_TARGET | correct target is centered and within stopping distance
Frame 8: EMERGENCY_STOP | boundary AprilTag visible
Frame 11: SEARCH_FOR_ROUTE | neither target nor route is visible
```

## Run tests

```bash
make test
```

Expected result:

```text
PASS: all controller decision situations
PASS: CSV camera simulation loader
```

## Simulated camera data

`simulated_camera_data.csv` contains one or more rows per frame:

```text
frame,valid,path_visible,path_offset,obstacle,bumper,tag_id,tag_angle_deg,tag_distance_m,tag_confidence
```

Multiple rows with the same frame number represent multiple AprilTags visible in one camera frame. A `tag_id` of `-1` means no tag was detected on that row.

The sample data demonstrates route following, correct and incorrect plant tags, boundary priority, obstacles, bumper activation, invalid frames, lost routes, weak detections, and unknown tags. Dock behavior is covered by the controller test suite.

## Repository structure

```text
.
├── main.cpp                       # Simulator entry point
├── rover_controller.h/.cpp        # Tag classification and control decisions
├── simulation_io.h/.cpp           # CSV frame loader
├── simulated_camera_data.csv      # Example observations
├── tests.cpp                      # Controller behavior tests
├── simulation_io_tests.cpp        # CSV loader tests
├── docs/
│   └── camera-frame-interface.md  # Planned perception interface
├── .github/workflows/ci.yml       # Automated build and tests
└── Makefile
```

## Physical safety limitation

Boundary AprilTags are not a continuous barrier. A physical rover must also use an independent continuous boundary or low perimeter lip, plus a bumper or equivalent hardware stop mechanism. Camera logic must not be the only protection against a ledge.

## Next milestone

Build a Python/OpenCV adapter that reads a real camera, detects AprilTags, and produces the same `CameraFrame` fields currently supplied by the CSV simulator.
