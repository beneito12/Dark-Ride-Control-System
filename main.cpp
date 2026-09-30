#include <iostream>
#include "Block.h"
#include "VehicleController.h"

const char* vehicleStatetoString(VehicleState state) {
    switch (state) {
        case VehicleState::STOPPED:
            return "STOPPED";
        case VehicleState::MOVING:
            return "MOVING";
        case VehicleState::READY_FOR_DISPATCH:
            return "READY_FOR_DISPATCH";
        case VehicleState::FAULTED:
        return "FAULTED";
        case VehicleState::EMERGENCY_STOP:
            return "EMERGENCY_STOP";
        default:
            return "UNKNOWN";
    }
}

int main() {
    std::cout << "Virtual Attraction Control System\n\n";

    Block block0(0);

    VehicleController vehicle1(1);

    bool granted = block0.requestEntry(vehicle1.getVehicleId());

    if (granted) {
        vehicle1.setCurrentBlock(block0.getId());
        vehicle1.setState(VehicleState::MOVING);
    }

    std::cout << "Vehicle " << vehicle1.getVehicleId() << " entry request: " << (granted ? "Granted" : "Denied") << std::endl;

    std::cout << "Vehicle " << vehicle1.getVehicleId() << " current block: " << vehicle1.getCurrentBlock() << std::endl;
    std::cout << "Vehicle " << vehicle1.getVehicleId() << " state: " << vehicleStatetoString(vehicle1.getState()) << std::endl;

    return 0;
}