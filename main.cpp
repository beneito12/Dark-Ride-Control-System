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
    std::cout
        << "Vehicle 1 | Block: "
        << ride.getVehicleCurrentBlock(0)
        << " | State: "
        << vehicleStateToString(ride.getVehicleState(0))
        << '\n';

    std::cout
        << "Vehicle 2 | Block: "
        << ride.getVehicleCurrentBlock(1)
        << " | State: "
        << vehicleStateToString(ride.getVehicleState(1))
        << '\n';

    std::cout
        << "Communication Fault: "
        << (ride.isCommunicationFaultActive() ? "ACTIVE" : "CLEAR")
        << '\n';

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

    RideController ride;

    // Configure normal station safety permissives.
    ride.setStationGateClosed(true);
    ride.setStationRestraintsLocked(true);
    ride.setStationEmergencyStopActive(false);

    auto runStep = [&](const std::string& description)
    {
        std::cout << "\n--- " << description << " ---\n";

        ride.update();
        printDemoStatus(ride);
    };

    // ----------------------------------------------------
    // PHASE 1: NORMAL DISPATCH AND BLOCK MOVEMENT
    // ----------------------------------------------------

    std::cout << "\n=== PHASE 1: NORMAL OPERATION ===\n";

    ride.requestStationDispatch();

    runStep("Operator requests dispatch");

    runStep("Station dwell cycle 1");
    runStep("Station dwell cycle 2");
    runStep("Vehicle advances from station");

    runStep("Track block dwell cycle");
    runStep("Vehicle enters show block");

    // ----------------------------------------------------
    // PHASE 2: SHOW CONTROL + COMMUNICATION FAILURE
    // ----------------------------------------------------

    std::cout << "\n=== PHASE 2: SHOW CONTROL + CAN FAULT ===\n";

    std::cout
        << "\nInjecting communication fault: "
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

    return 0;
}