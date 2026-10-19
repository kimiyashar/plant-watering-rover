# Camera-to-controller interface

The current simulator separates perception from control. A future Python/OpenCV process will convert camera images into the same structured observations that the C++ controller already consumes.

## `CameraFrame`

Each frame contains:

| Field | Meaning |
|---|---|
| `frameNumber` | Monotonic frame identifier |
| `valid` | Whether the frame is current and usable |
| `pathVisible` | Whether the route is visible |
| `pathOffset` | Horizontal route offset; negative is left, positive is right |
| `obstacleDetected` | Camera-level obstacle signal |
| `bumperPressed` | Physical safety signal, currently simulated |
| `tags` | Zero or more AprilTag detections |

## `TagDetection`

| Field | Meaning |
|---|---|
| `id` | AprilTag identifier |
| `angleDeg` | Horizontal target angle; negative is left |
| `distanceM` | Estimated camera-to-tag distance in meters |
| `confidence` | Detection confidence from 0 to 1 |

## Planned real-camera adapter

The planned Python/OpenCV component will:

1. Capture a camera frame on the Raspberry Pi.
2. Detect AprilTags and estimate their relative poses.
3. Estimate route position and detect obstacles.
4. Serialize a `CameraFrame` observation.
5. Send the observation to the C++ controller.
6. Receive a high-level action such as `TURN_LEFT` or `STOP_AT_TARGET`.

The transport between Python and C++ has not been selected or implemented yet. Possible options include newline-delimited JSON over a local process pipe, a lightweight socket, or direct C++ camera integration.

## Safety note

AprilTag boundaries are semantic warnings, not continuous physical barriers. The physical rover will require an independent continuous boundary or perimeter mechanism and a hardware-level stop path.
