#include "RideController.h"
#include <iostream>

RideController::RideController() : block0(0), vehicle1(1) {}

bool RideController::dispatchVehicle() {
    bool granted = block0.requestEntry(vehicle1.getVehicleId());

    if (granted) {
        vehicle1.setCurrentBlock(block0.getId());
        vehicle1.setState(VehicleState::Moving);
    }
    return granted;
}

void RideController::printStatus() const {
    std::cout << "Ride Status: "
    std::cout << "Vehicle " << vehicle1.getVehicleId() << std::endl << "Current Block: " << vehicle1.getCurrentBlock() << std::endl;
}