# VACS System Architecture

## Overview

The Virtual Attraction Control System is organized as a set of cooperating C++ controllers rather than one monolithic ride simulation.

The architecture separates vehicle behavior, station logic, block protection, communication, show control, and supervisory control into independent components.

```text
RideController
├── StationController
├── ShowController
├── CanBus
├── Block[]
└── VehicleController[]
```

`RideController` coordinates the system while each component owns a specific responsibility.

---

## RideController

`RideController` is the supervisory controller.

It manages:

- Vehicle fleet creation
- Control-cycle execution
- Station dispatch
- Vehicle movement
- Block transitions
- Communication watchdogs
- Communication fault recovery
- Emergency-stop behavior
- Show triggering
- Event logging

The fleet size is configurable:

```cpp
RideController ride(4);
```

Vehicle IDs are generated starting at 1.

---

## VehicleController

Each vehicle maintains its own state.

Tracked information includes:

- Vehicle ID
- Current block
- Vehicle state
- Time in current block
- Heartbeat transmission status

Vehicle states include:

```cpp
STOPPED
WAITING_FOR_DISPATCH
READY_FOR_DISPATCH
MOVING
COMPLETED_CYCLE
FAULTED
EMERGENCY_STOP
```

Vehicles transmit heartbeat and status messages through the simulated CAN bus.

---

## Block System

A `Block` represents one exclusive ride-path section.

Each block stores:

```text
Block ID
Occupying Vehicle ID
Required Dwell Time
```

Current configuration:

| Block | Function | Dwell |
|---|---|---:|
| 0 | Station | 2 ticks |
| 1 | Track | 1 tick |
| 2 | Show Scene | 3 ticks |

A vehicle must successfully request the next block before leaving its current block.

```text
Current Block Occupied
        |
Request Next Block
        |
   +----+----+
   |         |
Granted    Denied
   |         |
Move      Remain
Forward   In Place
```

The current block is released only after the next block grants entry.

---

## StationController

The station controller owns four primary conditions:

```text
gateClosed
restraintsLocked
emergencyStopActive
dispatchRequested
```

The station safety permissive is:

```cpp
gateClosed &&
restraintsLocked &&
!emergencyStopActive
```

The operator dispatch request is handled separately.

This distinction prevents safe conditions alone from initiating vehicle movement.

---

## CAN Communication Model

`CanBus` acts as a simulated broadcast communication bus.

Each message uses:

```cpp
struct CanMessage
{
    uint32_t id;
    std::array<uint8_t, 8> data;
    uint8_t length;
};
```

The message log is cleared at the beginning of each control cycle.

Each vehicle then transmits:

```text
Heartbeat
Status
```

The protocol uses base CAN identifiers:

```cpp
VEHICLE_HEARTBEAT_BASE_ID = 0x100
VEHICLE_STATUS_BASE_ID    = 0x200
SHOW_TRIGGER_ID           = 0x300
```

Vehicle-specific identifiers are generated from the vehicle ID.

Example:

```text
Vehicle 4 heartbeat = 0x104
Vehicle 4 status    = 0x204
```

---

## Watchdog Architecture

`RideController` maintains a missed-heartbeat counter for every configured vehicle.

For each update:

```text
Expected heartbeat found?
        |
   +----+----+
   |         |
  Yes        No
   |         |
Reset      Increment
counter    counter
             |
      Threshold reached?
             |
            Yes
             |
       Latch fault
```

The timeout threshold is three missed heartbeats.

The controller also records the vehicle ID responsible for the active communication fault.

---

## ShowController

The show subsystem uses two state concepts.

Show state:

```text
IDLE
RUNNING
COMPLETE
```

Active cue:

```text
NONE
LIGHTING
ANIMATRONIC
AUDIO
```

When a vehicle enters Block 2:

1. A `0x300` show-trigger message is transmitted.
2. `ShowController::startShow()` is called.
3. The cue sequence progresses during future control cycles.

The sequence can be retriggered for subsequent vehicles.

---

## Main Control Cycle

`RideController::update()` executes the system in a deliberate order.

```text
1. Increment simulation tick

2. Clear previous CAN messages

3. Vehicles transmit:
      - heartbeat
      - status

4. Run heartbeat watchdog

5. If communication fault active:
      stop normal update

6. If E-stop active:
      stop normal update

7. Update show controller

8. Convert completed vehicles
   back to waiting state

9. Process vehicle block movement

10. Process station dispatch
```

This ordering allows a vehicle to leave the station block before a waiting vehicle attempts to enter it during the same update.

---

## Multi-Vehicle Pipeline

Because movement and block occupancy are independent for each vehicle, multiple vehicles can occupy different blocks concurrently.

Example:

```text
Block 0        Block 1        Block 2
Vehicle 4      Vehicle 3      Vehicle 2

Vehicle 1 -> Completed Cycle
```

With three physical blocks, at most three vehicles may occupy the ride path at one time.

Additional configured vehicles remain waiting for dispatch.

---

## Event Logging

`RideController` records significant events using the current simulation tick.

Examples include:

```text
Vehicle entered show block
Communication fault detected
Communication reset rejected
Communication reset accepted
Emergency stop activated
Emergency stop cleared
Emergency stop reset accepted
```

The log provides a chronological record of state transitions and fault recovery.

---

## Testing and CI

The project uses Catch2 unit and integration tests.

CTest exposes the test executable to CMake:

```bash
ctest --test-dir build --output-on-failure
```

GitHub Actions performs the same configure, build, and test process in a clean Ubuntu environment.

This verifies that the system does not depend only on the developer's local machine.

---

## Design Principles

The architecture emphasizes:

- Separation of responsibilities
- Explicit state machines
- Deterministic update order
- Conservative fault behavior
- Preserved vehicle position during faults
- Explicit reset requirements
- Scalable vehicle count
- Testable interfaces
- Observable system behavior

The result is intended to demonstrate control-system software architecture rather than simply animate simulated ride vehicles.