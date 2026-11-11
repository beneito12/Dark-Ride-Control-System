# VACS Safety and Fault Handling

## Scope

VACS models several safety-oriented control concepts commonly used in automation and distributed control systems.

These include:

- Interlocks
- Exclusive block occupancy
- Emergency-stop behavior
- Communication watchdogs
- Latched faults
- Explicit reset requirements
- Position preservation
- Fault diagnostics

This is an educational software simulation, not a certified attraction safety system.

---

## Block Protection

Each ride block can be occupied by only one vehicle.

Before a vehicle moves:

```text
Vehicle requests next block
          |
          v
Is next block available?
       /       \
     Yes        No
      |          |
   Enter      Remain in
   block      current block
```

The current block is not released until the next block has granted entry.

This prevents two vehicles from being assigned to the same block.

---

## Dispatch Interlocks

The modeled station permissive requires:

```text
Gate Closed
AND
Restraints Locked
AND
Emergency Stop Not Active
```

A separate operator dispatch command is also required.

Therefore:

```text
Safe Station != Automatic Dispatch
```

Dispatch requests are consumed when an attempt is made.

If dispatch is requested while an interlock is unsafe, the request does not remain stored for later use.

The operator must issue a new command.

---

## Emergency Stop

When E-stop becomes active:

- Pending dispatch requests are cleared.
- Vehicles occupying blocks enter `EMERGENCY_STOP`.
- Block ownership remains unchanged.
- Vehicle movement stops.

Example:

```text
Before E-stop:

Block 0 -> Vehicle 4
Block 1 -> Vehicle 3
Block 2 -> Vehicle 2


After E-stop:

Block 0 -> Vehicle 4 | EMERGENCY_STOP
Block 1 -> Vehicle 3 | EMERGENCY_STOP
Block 2 -> Vehicle 2 | EMERGENCY_STOP
```

The controller does not erase the known vehicle positions.

---

## E-Stop Recovery

Clearing the E-stop condition is not sufficient to restart the ride.

The sequence is:

```text
E-stop Active
      |
      v
Motion Inhibited
      |
      v
E-stop Condition Cleared
      |
      v
Vehicles Remain Stopped
      |
      v
Explicit Reset
      |
      v
Motion May Resume
```

This prevents motion from restarting automatically after the external stop condition disappears.

---

## Communication Watchdog

Every vehicle is expected to send a heartbeat each control cycle.

The supervisory controller tracks missed heartbeats separately for each vehicle.

```text
Heartbeat Received
      |
     Yes
      |
Counter = 0
```

If not received:

```text
Missed Heartbeat
      |
Counter + 1
      |
3 Consecutive Misses
      |
Communication Fault
```

At the timeout threshold:

- The affected vehicle enters `FAULTED`.
- The responsible vehicle ID is recorded.
- A communication fault becomes active.
- Normal ride movement is inhibited.

---

## Latched Communication Fault

The communication fault remains active even if heartbeat communication later returns.

This creates the following recovery sequence:

```text
Heartbeat Timeout
      |
      v
Fault Latched
      |
      v
Heartbeat Restored
      |
      v
Fault STILL Active
      |
      v
Explicit Reset
      |
      v
Fault Cleared
```

If the controller still cannot verify all expected heartbeats, the reset is rejected.

---

## Communication Reset

A successful communication reset performs three actions:

```text
1. Clear communicationFaultActive

2. Clear communicationFaultVehicleId

3. Reset all missed-heartbeat counters
```

Faulted vehicles are then restored to an appropriate state.

If the vehicle is still inside a ride block:

```text
FAULTED -> MOVING
```

If it is not occupying a block:

```text
FAULTED -> WAITING_FOR_DISPATCH
```

---

## Ride-Wide Fault Inhibit

When a communication fault becomes active, the supervisory controller stops normal movement processing.

Other vehicles may still retain the `MOVING` state internally, but their positions do not advance while the global fault is active.

Example:

```text
Vehicle 2 | Block 2 | FAULTED
Vehicle 3 | Block 1 | MOVING
Vehicle 4 | Block 0 | MOVING

Communication Fault: ACTIVE
```

Vehicles 3 and 4 remain physically assigned to their blocks until the fault is cleared.

---

## Position Preservation

VACS intentionally preserves block ownership during:

```text
Emergency Stop
Communication Fault
```

This maintains a known system state during abnormal conditions.

A vehicle's block is not released merely because movement has stopped.

---

## Show Behavior During Faults

The show controller is updated only after communication and E-stop checks.

If a communication fault becomes active:

```text
Watchdog detects fault
        |
        v
RideController exits update
        |
        v
Vehicle movement pauses
Show progression pauses
```

Normal progression resumes after a successful fault reset.

---

## Fault Diagnostics

The controller exposes:

```text
Communication fault active/clear
Fault vehicle ID
Missed heartbeat count
```

Example:

```text
Communication Fault: ACTIVE
Source: Vehicle 2
Missed Heartbeats: 3
```

This provides more diagnostic information than a single Boolean fault indicator.

---

## Event Traceability

Significant events are recorded with simulation tick numbers.

Example:

```text
[Tick 6] Vehicle 1 entered Show Block - show triggered
[Tick 10] Vehicle 2 entered Show Block - show triggered
[Tick 13] Communication fault detected for vehicle 2
[Tick 13] Communication fault reset failed: Vehicle 2 heartbeat missing
[Tick 14] Communication fault reset accepted
[Tick 15] Emergency stop activated
[Tick 16] Emergency stop cleared
[Tick 17] Emergency stop reset accepted
```

This allows the sequence leading to and recovering from a fault to be reconstructed.

---

## Automated Safety Tests

The automated test suite verifies behavior including:

- Unsafe station conditions prevent dispatch
- Dispatch requires an operator request
- Dispatch requests do not remain latched
- Occupied blocks reject another vehicle
- Multi-vehicle separation is maintained
- E-stop halts ride movement
- E-stop preserves block occupancy
- Clearing E-stop does not restart motion automatically
- Explicit reset restores eligible vehicle motion
- Missing heartbeats increment watchdog counters
- Three missed heartbeats activate a fault
- Fault diagnostics identify the correct vehicle
- Communication faults inhibit movement
- Communication faults remain latched after heartbeat recovery
- Reset fails while required communication is missing
- Successful reset restores faulted vehicles
- Successful reset clears watchdog counters

Tests run locally with Catch2/CTest and automatically through GitHub Actions.

---

## Limitations

VACS does not implement:

- Safety PLC hardware
- Redundant processors
- Safety-rated networks
- Hardware emergency-stop circuits
- SIL or PL certification
- Formal hazard analysis
- Fail-safe electrical design
- Certified attraction-control standards
- Real vehicle propulsion hardware

Those features are outside the scope of this software portfolio project.

The safety logic in VACS is intended to demonstrate software architecture and control concepts only.