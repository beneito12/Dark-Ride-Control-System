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