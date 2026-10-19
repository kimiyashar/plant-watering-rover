# Physical Rover Hardware Plan

This bill of materials converts the current software-only controller into a small indoor plant-watering rover. The intended first physical milestone is deliberately narrow:

> Drive away from the home bay, detect one plant AprilTag, stop at the plant without crossing the boundary, and return home. Water is added only after navigation is reliable.

## Recommended architecture

```text
Raspberry Pi 5
  Python + OpenCV + AprilTag detection
                 |
           USB serial
                 v
Romi 32U4 controller
  C++ low-level control + encoders + safety inputs
                 |
          motors / pump switch
```

The Raspberry Pi handles images and high-level perception. The Romi 32U4 handles timing-sensitive motor control and remains able to stop the motors if vision or Pi communication fails.

## 1. Drive parts

### Buy for the first milestone

| Qty | Part | Purpose | Recommended item |
|---:|---|---|---|
| 1 | Differential-drive chassis | Base, wheels, motors, caster, and AA battery contacts | [Pololu Romi Chassis Kit, black](https://www.pololu.com/product/3500) |
| 1 | Low-level motor controller | Dual motor drivers, C++-programmable MCU, IMU, encoder inputs, and Pi interface | [Pololu Romi 32U4 Control Board](https://www.pololu.com/product/3544) |
| 1 | Encoder pair | Closed-loop speed, straight driving, distance, and turn estimation | [Pololu Romi Encoder Pair Kit](https://www.pololu.com/product/3542) |
| 6 | Rechargeable AA NiMH cells | Drive-system power | 1.2 V low-self-discharge NiMH cells, 1900-2500 mAh |
| 1 | Smart NiMH charger | Safe charging outside the rover | Charger supporting six cells, or two four-slot chargers |
| 1 | USB-A to Micro-B data cable | Programming the Romi 32U4 | Must support data, not charge only |

### Why this base

- It can rotate in place, which suits the compact plant arrangement.
- The chassis kit already includes both 120:1 gearmotors, wheels, caster, and battery contacts.
- Encoders make repeatable slow approaches possible.
- The 32U4 keeps motor timing and emergency stopping independent from Linux and the camera process.

### Verified vendor prices on October 19, 2026

- Romi chassis: **$39.95**
- Romi 32U4 board: **$74.95**
- Encoder pair: **$9.95**
- Core Pololu drive hardware subtotal: **$124.85**, before batteries, charger, tax, and shipping.

## 2. Computer-vision and compute parts

### Buy after the base drives under encoder control

| Qty | Part | Purpose | Recommendation |
|---:|---|---|---|
| 1 | Raspberry Pi 5, 4 GB | Python/OpenCV, AprilTags, logging, and later plant-health inference | 4 GB is sufficient for this prototype; 8 GB is unnecessary |
| 1 | Raspberry Pi Active Cooler | Keeps the Pi from throttling during continuous vision | Official Pi 5 Active Cooler |
| 1 | Raspberry Pi Camera Module 3 | Autofocus RGB images for tags and plant images | Standard-angle, regular RGB version, not NoIR |
| 1 | Pi 5-compatible camera cable | Connects Camera Module 3 to the Pi 5's smaller FFC connector | Confirm it is the mini 22-pin-to-15-pin cable |
| 1 | 32-64 GB high-endurance microSD card | OS, code, and logs | A2-rated card preferred |
| 1 | USB-C power bank or regulated 5 V supply rated for Pi 5 | Separate clean power for compute | Use a reputable USB-C PD source |
| 1 | Short USB data cable | Pi-to-32U4 serial link | USB-A/USB-C to Micro-B as appropriate |
| 1 | Adjustable camera bracket | Sets camera height and downward angle | 3D-print or use a small metal camera bracket |

### Camera choice

Use the **standard Camera Module 3**, not the wide-angle or NoIR version, for the first build. It gives less edge distortion for AprilTag pose estimation and better color information for later plant-health work. Mount it near the rover centerline, roughly 15-25 cm above the floor, angled slightly downward.

Do not power the Pi 5 from the Romi board's 5 V rail. The Romi board is rated for 2 A continuous on that rail, while the Pi 5 can require substantially more. Use separate Pi power and connect the systems through USB serial.

## 3. Safety and navigation sensors

### Required before unsupervised movement

| Qty | Part | Purpose | Recommendation |
|---:|---|---|---|
| 2 | Lever microswitches with rollers or whiskers | Left/right physical bumper | Normally closed wiring is preferred so a broken wire becomes a fault |
| 1 | Front distance sensor | Detects pots, feet, and objects before contact | VL53L1X time-of-flight breakout |
| 1 | Three-channel downward reflectance array | Detects continuous floor boundary tape | Pololu QTR-3RC or equivalent |
| 1 roll | Matte contrasting floor tape | Continuous boundary that cannot be missed between tags | Black tape on a light floor or white tape on a dark floor |
| 1 | Latching power switch / motor kill | Immediate physical shutdown | Accessible from the top or rear |

AprilTags at the four corners are **not** a sufficient boundary. They are semantic landmarks, not a continuous barrier. Add a continuous tape line, a low physical lip, or both. The downward reflectance sensors should stop the motors directly through the low-level controller.

## 4. AprilTags and station fixtures

| Qty | Part | Purpose |
|---:|---|---|
| 5-10 | Printed plant tags, IDs `1-99` | Identify individual plants |
| 1 | Printed dock tag, ID `500` | Identify the home bay |
| 4+ | Printed boundary tags, IDs `900-999` | Semantic warning at corners and entrances |
| As needed | Rigid matte backing | Keeps each tag flat and readable |
| 1 per plant | Pot-mounted tag stake or bracket | Holds the tag at a repeatable height and angle |
| 1 per plant | Watering funnel or docking guide | Mechanically tolerates small stopping errors |

Start with tags around **8-10 cm square**. Print them with a clean white margin on matte paper and mount them rigidly, facing the rover's expected approach. Do not attach tags to leaves.

## 5. Water-delivery parts

### Buy only after one-station navigation is repeatable

| Qty | Part | Purpose | Recommendation |
|---:|---|---|---|
| 1 | Small peristaltic pump | Keeps water isolated inside tubing and enables timed dosing | 6 V pump compatible with the drive battery range |
| 1 | Logic-level MOSFET switch module | Lets the controller switch the pump | Must include or support a flyback diode |
| 1 | Flyback diode | Protects electronics from pump-motor voltage spikes | Required if not already present on the switch module |
| 1 | 250-500 mL leak-resistant reservoir | Carries test water | Start at 250 mL to limit mass and spill risk |
| 1-2 m | Pump-compatible silicone tubing | Water path | Match the pump's specified inner and outer diameter |
| 1 | Check valve | Reduces siphoning and dripping | Match tubing size |
| 1 | Rigid dispensing nozzle | Repeatable outlet position | Mount low and forward |
| 1 | Removable electronics splash shield / drip tray | Protects compute and wiring | 3D-printed or formed plastic |

For initial tests, use an empty pot or catch cup and a small measured volume. Keep the reservoir below the electronics, use strain relief on every tube connection, and never test leaks over powered electronics.

## 6. Power and wiring

| Qty | Part | Purpose |
|---:|---|---|
| 1 | Small solderable prototyping board | Sensor and pump connections |
| 1 set | 22-26 AWG stranded hookup wire | Power and signal wiring |
| 1 set | Heat-shrink tubing | Insulates solder joints |
| 1 set | JST or locking connectors | Allows modules to be removed safely |
| 1 | Inline fuse and holder | Protects the battery branch feeding added actuators |
| 1 | Common-ground connection between control subsystems | Establishes signal reference when required |
| Several | Ferrules, cable ties, and adhesive cable mounts | Strain relief and cable management |

Keep motor/pump wiring physically separated from the camera cable. The Pi and drive system use separate power sources for the first prototype.

## 7. Mechanical mounting

| Qty | Part | Purpose |
|---:|---|---|
| 1-2 | Romi expansion plates or custom 3D-printed deck | Holds Pi, camera mast, and reservoir |
| 1 set | M2/M2.5/M3 screws, nuts, and standoffs | Mounts boards and brackets |
| 1 | Camera mast or bracket | Gives the camera a clear view over the chassis |
| 1 | Reservoir cradle with strap | Prevents water mass from shifting during turns |
| 1 | Front bumper bar | Couples both microswitches to a broad contact surface |
| 1 | Simple home-bay funnel/guide rails | Makes visual docking mechanically repeatable |

Keep the reservoir low and near the center. Keep the camera rigid and above the reservoir. Do not add automatic charging contacts in version 1; the home bay is initially a visual parking station only.

## 8. Tools and consumables

- Soldering iron and lead-free or appropriately handled solder
- Wire stripper and small flush cutters
- Multimeter
- Small screwdrivers and hex keys
- Calipers or ruler
- Double-sided foam tape and hook-and-loop straps
- Access to a 3D printer, optional but useful
- Plastic tray and paper towels for watering tests

## Purchase order

### Order 1: make it drive

1. Romi chassis kit
2. Romi 32U4 control board
3. Romi encoder pair
4. Six NiMH AA cells and charger
5. Micro-B data cable

**Acceptance test:** command slow forward motion, closed-loop straight driving, in-place left/right turns, bumper stop, and encoder-reported distance.

### Order 2: replace simulated vision

1. Raspberry Pi 5, 4 GB
2. Active Cooler
3. Camera Module 3 and Pi 5 camera cable
4. microSD card
5. USB-C power source
6. Camera bracket
7. Printed AprilTags

**Acceptance test:** detect one real plant tag, publish its ID/angle/distance, and make the existing controller stop at it.

### Order 3: make navigation physically safe

1. Two bumper switches
2. VL53L1X front distance sensor
3. QTR-3RC downward boundary sensor
4. Contrasting boundary tape
5. Latching motor kill switch

**Acceptance test:** every sensor independently stops the motors, including when the Pi process is stopped.

### Order 4: add water

1. Peristaltic pump
2. MOSFET switch and flyback protection
3. 250 mL reservoir
4. Tubing, check valve, and nozzle
5. Splash shield and pot funnels

**Acceptance test:** dispense a calibrated amount into a catch cup for repeated trials without drips or leaks.

## Intentionally deferred

- Automatic charging contacts
- LiPo battery packs
- Full-room SLAM or lidar
- RFID/NFC readers
- Robotic arm
- Large water tank
- Plant-health accelerator hardware

These add complexity without helping the first end-to-end demonstration.