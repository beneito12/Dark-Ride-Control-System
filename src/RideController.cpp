#include "RideController.h"
#include <iostream>

RideController::RideController() : block0(0), block1(1), block2(2), vehicle1(1) {}

bool RideController::dispatchVehicle() {
    bool granted = block0.requestEntry(vehicle1.getVehicleId());

    if (granted) {
        vehicle1.setCurrentBlock(block0.getId());
        vehicle1.setState(VehicleState::MOVING);
    }
    return granted;
}

bool RideController::moveVehicleToNextBlock() {
    int currentBlock = vehicle1.getCurrentBlock();

    if (currentBlock == 0) {
        bool granted = block1.requestEntry(vehicle1.getVehicleId());
        if (granted) {
            block0.release();
            vehicle1.setCurrentBlock(1);
        }
        return granted;
    }
    return false;
}

void RideController::printStatus() const {
    std::cout << "Ride Status: " << std::endl;
    std::cout << "Vehicle " << vehicle1.getVehicleId() << std::endl << "Current Block: " << vehicle1.getCurrentBlock() << std::endl;
}