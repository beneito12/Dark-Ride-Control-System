#include "RideController.h"
#include <iostream>

RideController::RideController() : vehicle1(1) {\
    blocks.emplace_back(0);
    blocks.emplace_back(1);
    blocks.emplace_back(2);
}

bool RideController::dispatchVehicle() {
    bool granted = blocks[0].requestEntry(vehicle1.getVehicleId());

    if (granted) {
        vehicle1.setCurrentBlock(blocks[0].getId());
        vehicle1.setState(VehicleState::MOVING);
    }
    return granted;
}

bool RideController::moveVehicleToNextBlock() {
    int currentBlock = vehicle1.getCurrentBlock();
    int nextBlock = currentBlock + 1;
    
    if (currentBlock < 0) return false;

    if (nextBlock >= static_cast<int>(blocks.size())) return false;

    bool granted = blocks[nextBlock].requestEntry(vehicle1.getVehicleId());
    if (granted) {
        blocks[currentBlock].release();
        vehicle1.setCurrentBlock(nextBlock);
    }
    return granted;
}

void RideController::printStatus() const {
    std::cout << "Ride Status: " << std::endl;
    std::cout << "Vehicle " << vehicle1.getVehicleId() << std::endl << "Current Block: " << vehicle1.getCurrentBlock() << std::endl;

    std::cout << "Block 0: " << (blocks[0].isOccupied() ? "OCCUPIED " : "CLEAR") << std::endl;
    if (blocks[0].isOccupied()) {
        std::cout << " by vehicle " << blocks[0].getOccupyingVehicle() << std::endl;
    }
    std::cout << "Block 1: " << (blocks[1].isOccupied() ? "OCCUPIED" : "CLEAR") << std::endl;
    if (blocks[1].isOccupied()) {
        std::cout << " by vehicle " << blocks[1].getOccupyingVehicle() << std::endl;
    }
    std::cout << "Block 2: " << (blocks[2].isOccupied() ? "OCCUPIED" : "CLEAR") << std::endl;
    if (blocks[2].isOccupied()) {
        std::cout << " by vehicle " << blocks[2].getOccupyingVehicle() << std::endl;
    }
}