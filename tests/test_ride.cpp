#include <catch2/catch_test_macros.hpp>
#include "RideController.h"

TEST_CASE("Emergency stop halts a vehicle already on the ride")
{
    RideController ride;

    // Make the station dispatch-permissive.
    ride.setStationGateClosed(true);
    ride.setStationRestraintsLocked(true);
    ride.setStationEmergencyStopActive(false);

    // Simulate the operator pressing dispatch.
    ride.requestStationDispatch();

    REQUIRE(ride.dispatchVehicle(0));

    // Vehicle 1 should start in Block 0.
    REQUIRE(ride.getVehicleCurrentBlock(0) == 0);

    // Block 0 requires 2 dwell ticks.
    ride.update();
    ride.update();
    ride.update(); // Third update to complete the dwell time in Block 0

    // Vehicle should now have advanced to Block 1.
    REQUIRE(ride.getVehicleCurrentBlock(0) == 1);

    // Hit the E-stop.
    ride.setStationEmergencyStopActive(true);
    REQUIRE(ride.isBlockOccupied(1)); // Block 1 should be occupied by the vehicle
    REQUIRE(ride.getBlockOccupyingVehicle(1) == 1); // Vehicle 1 should be occupying Block 1

    REQUIRE(ride.getVehicleState(0) == VehicleState::EMERGENCY_STOP);

    // Remember where the vehicle stopped.
    int stoppedBlock = ride.getVehicleCurrentBlock(0);

    // Several updates should NOT move it.
    ride.update();
    ride.update();
    ride.update();

    REQUIRE(ride.getVehicleCurrentBlock(0) == stoppedBlock);
}

TEST_CASE("Clearing emergency stop requires explicit reset before movement resumes")
{
    RideController ride;

    ride.setStationGateClosed(true);
    ride.setStationRestraintsLocked(true);
    ride.setStationEmergencyStopActive(false);

    ride.requestStationDispatch();
    REQUIRE(ride.dispatchVehicle(0));

    // Move Vehicle 1 into Block 1.
    ride.update();
    ride.update();
    ride.update();

    REQUIRE(ride.getVehicleCurrentBlock(0) == 1);

    // Activate E-stop.
    ride.setStationEmergencyStopActive(true);
    REQUIRE(ride.getVehicleState(0) == VehicleState::EMERGENCY_STOP);

    int stoppedBlock = ride.getVehicleCurrentBlock(0);

    // Clear the physical/logical E-stop condition.
    ride.setStationEmergencyStopActive(false);

    // Clearing it alone must NOT restart the vehicle.
    ride.update();
    ride.update();

    REQUIRE(ride.getVehicleCurrentBlock(0) == stoppedBlock);
    REQUIRE(ride.getVehicleState(0) == VehicleState::EMERGENCY_STOP);

    // Explicit reset is required.
    ride.resetEmergencyStop();

    REQUIRE(ride.getVehicleState(0) == VehicleState::MOVING);

    // Normal updates may now resume movement.
    ride.update();
    ride.update();

    REQUIRE(ride.getVehicleCurrentBlock(0) != stoppedBlock);
}

TEST_CASE("Vehicle cannot dispatch without operator dispatch request")
{
    RideController ride;

    ride.setStationGateClosed(true);
    ride.setStationRestraintsLocked(true);
    ride.setStationEmergencyStopActive(false);

    // Station is safe, but operator has not requested dispatch.
    ride.update();

    REQUIRE(ride.getVehicleCurrentBlock(0) == -1);
    REQUIRE_FALSE(ride.isBlockOccupied(0));
}

TEST_CASE("Vehicle dispatches when station is safe and operator requests dispatch")
{
    RideController ride;

    ride.setStationGateClosed(true);
    ride.setStationRestraintsLocked(true);
    ride.setStationEmergencyStopActive(false);

    ride.requestStationDispatch();
    ride.update();

    REQUIRE(ride.getVehicleCurrentBlock(0) == 0);
    REQUIRE(ride.isBlockOccupied(0));
    REQUIRE(ride.getBlockOccupyingVehicle(0) == 1);
}

TEST_CASE("Unsafe dispatch request does not remain latched")
{
    RideController ride;

    // Unsafe condition: gate is open.
    ride.setStationGateClosed(false);
    ride.setStationRestraintsLocked(true);
    ride.setStationEmergencyStopActive(false);

    // Operator presses dispatch while unsafe.
    ride.requestStationDispatch();
    ride.update();

    REQUIRE(ride.getVehicleCurrentBlock(0) == -1);

    // Make the station safe afterward.
    ride.setStationGateClosed(true);

    // No new dispatch request has been made.
    ride.update();

    // Vehicle must still remain in the station.
    REQUIRE(ride.getVehicleCurrentBlock(0) == -1);

    // A fresh operator request should now work.
    ride.requestStationDispatch();
    ride.update();

    REQUIRE(ride.getVehicleCurrentBlock(0) == 0);
}

TEST_CASE("Second vehicle cannot dispatch into occupied station block")
{
    RideController ride;

    ride.setStationGateClosed(true);
    ride.setStationRestraintsLocked(true);
    ride.setStationEmergencyStopActive(false);

    // Dispatch Vehicle 1 into Block 0.
    ride.requestStationDispatch();
    REQUIRE(ride.dispatchVehicle(0));

    REQUIRE(ride.getVehicleCurrentBlock(0) == 0);
    REQUIRE(ride.isBlockOccupied(0));

    // Attempt to dispatch Vehicle 2 while Block 0 is occupied.
    ride.requestStationDispatch();

    REQUIRE_FALSE(ride.dispatchVehicle(1));

    // Vehicle 2 must remain off the ride.
    REQUIRE(ride.getVehicleCurrentBlock(1) == -1);

    // Block 0 must still belong to Vehicle 1.
    REQUIRE(ride.getBlockOccupyingVehicle(0) == 1);
}

TEST_CASE("All vehicles transmit CAN heartbeats and status messages during ride update")
{
    RideController ride;

    ride.update();

    const auto& messages = ride.getCanMessages();

   REQUIRE(messages.size() == 4);

    REQUIRE(messages[0].id == 0x101);
    REQUIRE(messages[1].id == 0x201);
    REQUIRE(messages[2].id == 0x102);
    REQUIRE(messages[3].id == 0x202);
}

TEST_CASE("Ride detects communication timeout after three missed heartbeats")
{
    RideController ride;

    // Vehicle index 1 = Vehicle 2.
    ride.setVehicleHeartbeatEnabled(1, false);

    ride.update();
    REQUIRE_FALSE(ride.isCommunicationFaultActive());

    ride.update();
    REQUIRE_FALSE(ride.isCommunicationFaultActive());

    ride.update();

    REQUIRE(ride.isCommunicationFaultActive());
    REQUIRE(ride.getVehicleState(1) == VehicleState::FAULTED);
}

TEST_CASE("Communication fault freezes ride motion and preserves block occupancy")
{
    RideController ride;

    ride.setStationGateClosed(true);
    ride.setStationRestraintsLocked(true);
    ride.setStationEmergencyStopActive(false);

    ride.requestStationDispatch();
    ride.update();

    REQUIRE(ride.getVehicleCurrentBlock(0) == 0);

    // Vehicle 2 stops communicating.
    ride.setVehicleHeartbeatEnabled(1, false);

    // Three missed heartbeats trigger the watchdog.
    ride.update();
    ride.update();
    ride.update();

    REQUIRE(ride.isCommunicationFaultActive());

    int stoppedBlock = ride.getVehicleCurrentBlock(0);

    REQUIRE(stoppedBlock >= 0);
    REQUIRE(ride.isBlockOccupied(stoppedBlock));
    REQUIRE(ride.getBlockOccupyingVehicle(stoppedBlock) == 1);

    // More control cycles must not move Vehicle 1.
    ride.update();
    ride.update();
    ride.update();

    REQUIRE(ride.getVehicleCurrentBlock(0) == stoppedBlock);
    REQUIRE(ride.isBlockOccupied(stoppedBlock));
    REQUIRE(ride.getBlockOccupyingVehicle(stoppedBlock) == 1);
}

TEST_CASE("Communication fault requires restored heartbeat and explicit reset")
{
    RideController ride;

    ride.setStationGateClosed(true);
    ride.setStationRestraintsLocked(true);
    ride.setStationEmergencyStopActive(false);

    ride.requestStationDispatch();
    ride.update();

    REQUIRE(ride.getVehicleCurrentBlock(0) == 0);

    // Simulate loss of Vehicle 2 communications.
    ride.setVehicleHeartbeatEnabled(1, false);

    ride.update();
    ride.update();
    ride.update();

    REQUIRE(ride.isCommunicationFaultActive());

    int stoppedBlock = ride.getVehicleCurrentBlock(0);

    // Reset must fail while communications are still missing.
    REQUIRE_FALSE(ride.resetCommunicationFault());

    // Restore Vehicle 2 communications.
    ride.setVehicleHeartbeatEnabled(1, true);

    // Vehicle now sends a heartbeat, but the ride remains faulted.
    ride.update();

    REQUIRE(ride.isCommunicationFaultActive());
    REQUIRE(ride.getVehicleCurrentBlock(0) == stoppedBlock);

    // Explicit reset may now succeed.
    REQUIRE(ride.resetCommunicationFault());
    REQUIRE_FALSE(ride.isCommunicationFaultActive());
}

TEST_CASE("Vehicle entering show block triggers show controller")
{
    RideController ride;

    ride.setStationGateClosed(true);
    ride.setStationRestraintsLocked(true);
    ride.setStationEmergencyStopActive(false);

    ride.requestStationDispatch();
    ride.update();

    REQUIRE(ride.getShowState() == ShowState::IDLE);

    // Block 0 requires 2 ticks before leaving.
    ride.update();
    ride.update();
    ride.update();

    REQUIRE(ride.getVehicleCurrentBlock(0) == 1);

    // Block 1 requires 1 tick.
    ride.update();
    ride.update();

    REQUIRE(ride.getVehicleCurrentBlock(0) == 2);
    REQUIRE(ride.getShowState() == ShowState::RUNNING);
}