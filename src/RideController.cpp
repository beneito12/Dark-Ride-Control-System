#include "RideController.h"
#include <iostream>

RideController::RideController() {
    blocks.emplace_back(0);
    blocks.emplace_back(1);
    blocks.emplace_back(2);
    vehicles.emplace_back(1);
    vehicles.emplace_back(2);
}

bool RideController::dispatchVehicle(int vehicleIndex) {
    VehicleController& vehicle = vehicles[vehicleIndex];

    bool granted = blocks[0].requestEntry(vehicle.getVehicleId());

    if (granted) {
        vehicle.setCurrentBlock(blocks[0].getId());
        vehicle.setState(VehicleState::MOVING);
    }
    return granted;
}

bool RideController::moveVehicleToNextBlock(int vehicleIndex) {
    VehicleController& vehicle = vehicles[vehicleIndex];

    int currentBlock = vehicle.getCurrentBlock();
    int nextBlock = currentBlock + 1;
    
    if (currentBlock < 0) return false;

    if (nextBlock >= static_cast<int>(blocks.size())) return false;

    bool granted = blocks[nextBlock].requestEntry(vehicle.getVehicleId());
    if (granted) {
        blocks[currentBlock].release();
        vehicle.setCurrentBlock(nextBlock);
    }
    return granted;
}

void RideController::printStatus() const {
    std::cout << "Ride Status: " << std::endl;

    for (const VehicleController& vehicle : vehicles) {
        std::cout << "Vehicle " << vehicle.getVehicleId() << std::endl << "Current Block: " << vehicle.getCurrentBlock() << std::endl;
    }

    for (const Block& block : blocks) {
        std::cout << "Block " << block.getId() << ": "; 
        if (block.isOccupied()) {
            std::cout << "OCCUPIED" << std::endl;
            std::cout << " by vehicle " << block.getOccupyingVehicle() << std::endl;
        } else {
            std::cout << "CLEAR" << std::endl;
        }
    }
}

int RideController::getVehicleId(int vehicleIndex) const {
    return vehicles[vehicleIndex].getVehicleId();
}