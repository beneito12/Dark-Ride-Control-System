#include "RideController.h"
#include <iostream>
#include <string>

const char* vehicleStateToString(VehicleState state)
{
    switch (state)
    {
        case VehicleState::STOPPED:
            return "STOPPED";

        case VehicleState::WAITING_FOR_DISPATCH:
            return "WAITING_FOR_DISPATCH";

        case VehicleState::READY_FOR_DISPATCH:
            return "READY_FOR_DISPATCH";

        case VehicleState::MOVING:
            return "MOVING";

        case VehicleState::COMPLETED_CYCLE:
            return "COMPLETED_CYCLE";

        case VehicleState::FAULTED:
            return "FAULTED";

        case VehicleState::EMERGENCY_STOP:
            return "EMERGENCY_STOP";
    }

    return "UNKNOWN";
}

const char* showStateToString(ShowState state)
{
    switch (state)
    {
        case ShowState::IDLE:
            return "IDLE";

        case ShowState::RUNNING:
            return "RUNNING";

        case ShowState::COMPLETE:
            return "COMPLETE";
    }

    return "UNKNOWN";
}

const char* showCueToString(ShowCue cue)
{
    switch (cue)
    {
        case ShowCue::NONE:
            return "NONE";

        case ShowCue::LIGHTING:
            return "LIGHTING";

        case ShowCue::ANIMATRONIC:
            return "ANIMATRONIC";

        case ShowCue::AUDIO:
            return "AUDIO";
    }

    return "UNKNOWN";
}

void printDemoStatus(const RideController& ride)
{
    for (int i = 0; i < ride.getVehicleCount(); i++)
{
    std::cout
        << "Vehicle " << ride.getVehicleId(i)
        << " | Block: " << ride.getVehicleCurrentBlock(i)
        << " | State: " << vehicleStateToString(ride.getVehicleState(i))
        << '\n';
}

    if (ride.isCommunicationFaultActive())
{
    int faultVehicleId = ride.getCommunicationFaultVehicleId();
    int faultVehicleIndex = faultVehicleId - 1;

    std::cout
        << "Communication Fault: ACTIVE"
        << " | Source: Vehicle " << faultVehicleId
        << " | Missed Heartbeats: "
        << ride.getMissedHeartbeatCount(faultVehicleIndex)
        << '\n';
}
else
{
    std::cout << "Communication Fault: CLEAR\n";
}

    std::cout
        << "Show State: "
        << showStateToString(ride.getShowState())
        << " | Cue: "
        << showCueToString(ride.getActiveShowCue())
        << '\n';

    std::cout
        << "CAN Frames This Cycle: "
        << ride.getCanMessages().size()
        << '\n';
}

int main()
{
    std::cout << "========================================\n";
    std::cout << " Virtual Attraction Control System\n";
    std::cout << " Distributed Controls Demonstration\n";
    std::cout << "========================================\n";

    RideController ride(4);

    // Configure normal station safety permissives.
    ride.setStationGateClosed(true);
    ride.setStationRestraintsLocked(true);
    emergencyStopActive = true;
    ride.setStationEmergencyStopActive(false);
    ride.resetEmergencyStop();  // mirrors real world operation where estop is cleared after power on

    auto runStep = [&](const std::string& description)
    {
        std::cout << "\n--- " << description << " ---\n";

        ride.update();
        printDemoStatus(ride);
    };

      // ----------------------------------------------------
      // PHASE 1: MULTI-VEHICLE NORMAL OPERATION
      // ----------------------------------------------------

      std::cout << "\n=== PHASE 1: MULTI-VEHICLE OPERATION ===\n";

      // Dispatch Vehicle 1.
      ride.requestStationDispatch();
      runStep("Dispatch Vehicle 1");

      runStep("Vehicle 1 station dwell - cycle 1");
      runStep("Vehicle 1 station dwell - cycle 2");

      // During this update Vehicle 1 leaves Block 0.
      // The pending dispatch request allows Vehicle 2 to enter it.
      ride.requestStationDispatch();
      runStep("Vehicle 1 advances; Vehicle 2 dispatches");

      // Vehicle 1 progresses toward the show block while Vehicle 2
      // occupies the station.
      runStep("Vehicles 1 and 2 advance through ride");

      // Vehicle 1 enters the show block.
      runStep("Vehicle 1 enters show block");

      // Vehicle 2 is now ready to leave the station.
      // Request Vehicle 3 before the same update.
      ride.requestStationDispatch();
      runStep("Vehicle 2 advances; Vehicle 3 dispatches");

      runStep("Three vehicles circulating");
      runStep("Block protection maintains vehicle separation");

      // Vehicle 1 exits, Vehicle 2 can enter the show block,
      // Vehicle 3 can enter Block 1, and Vehicle 4 can dispatch.
      ride.requestStationDispatch();
      runStep("Vehicle 4 dispatches as ride pipeline advances");

    // ----------------------------------------------------
    // PHASE 2: SHOW CONTROL + COMMUNICATION FAULT
    // ----------------------------------------------------

    std::cout << "\n=== PHASE 2: SHOW CONTROL + CAN FAULT ===\n";

    std::cout
        << "\nInjecting heartbeat fault: "
        << "Vehicle 2 heartbeat disabled.\n";

    ride.setVehicleHeartbeatEnabled(1, false);

    runStep("Vehicle 2 heartbeat missing - watchdog miss 1");
    runStep("Vehicle 2 heartbeat missing - watchdog miss 2");
    runStep("Vehicle 2 heartbeat missing - watchdog miss 3");

    // ----------------------------------------------------
    // PHASE 3: COMMUNICATION FAULT RECOVERY
    // ----------------------------------------------------

    std::cout << "\n=== PHASE 3: FAULT RECOVERY ===\n";

    bool resetAccepted = ride.resetCommunicationFault();

    std::cout
        << "\nReset attempted while communication is still missing: "
        << (resetAccepted ? "ACCEPTED" : "REJECTED")
        << '\n';

    std::cout << "Restoring Vehicle 2 heartbeat...\n";

    ride.setVehicleHeartbeatEnabled(1, true);

    runStep("Heartbeat restored - fault remains latched");

    resetAccepted = ride.resetCommunicationFault();

    std::cout
        << "\nExplicit communication fault reset: "
        << (resetAccepted ? "ACCEPTED" : "REJECTED")
        << '\n';

    runStep("Ride resumes after communication reset");

    // ----------------------------------------------------
    // PHASE 4: EMERGENCY STOP
    // ----------------------------------------------------

    std::cout << "\n=== PHASE 4: EMERGENCY STOP ===\n";

    std::cout << "\nActivating emergency stop...\n";

    ride.setStationEmergencyStopActive(true);

    printDemoStatus(ride);

    runStep("Control cycle while E-stop is active");

    std::cout << "\nClearing E-stop condition without reset...\n";

    ride.setStationEmergencyStopActive(false);

    runStep("E-stop cleared - vehicle remains stopped");

    std::cout << "\nIssuing explicit E-stop reset...\n";

    ride.resetEmergencyStop();

    runStep("Vehicle motion resumes after reset");
    runStep("Final control cycle");

    std::cout << "\n========================================\n";
    std::cout << " Demonstration Complete\n";
    std::cout << "========================================\n";

    std::cout << "\n=== EVENT LOG ===\n";

   for (const std::string& event : ride.getEventLog())
      {
       std::cout << event << '\n';
      }

    return 0;
}