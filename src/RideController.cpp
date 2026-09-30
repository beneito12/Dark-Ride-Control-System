#include "RideController.h"
#include <iostream>

void RideController::update() {
    for (int i = 0; i < static_cast<int>(vehicles.size()); i++) {
        int currentBlock = vehicles[i].getCurrentBlock();
        if (currentBlock >= 0) {
           int requiredTicks = blocks[currentBlock].getRequiredTicks();

           if (vehicles[i].getTicksInCurrentBlock() >= requiredTicks) {
               moveVehicleToNextBlock(i);
           }
           else {
               vehicles[i].incrementBlockTimer();
           }
        }
    }

    for (int i = 0; i < static_cast<int>(vehicles.size()); i++) {
        if (vehicles[i].getCurrentBlock() == -1) {
            dispatchVehicle(i);
        }
    }
}

RideController::RideController() {
    blocks.emplace_back(0, 2); // station
    blocks.emplace_back(1, 1); // track
    blocks.emplace_back(2, 3); // show scene
    vehicles.emplace_back(1);
    vehicles.emplace_back(2);
}

bool RideController::dispatchVehicle(int vehicleIndex) {
    if (vehicleIndex < 0 || vehicleIndex >= static_cast<int>(vehicles.size())) {
        return false;
    }
    VehicleController& vehicle = vehicles[vehicleIndex];

    bool granted = blocks[0].requestEntry(vehicle.getVehicleId());

    if (granted) {
        vehicle.setCurrentBlock(blocks[0].getId());
        vehicle.setState(VehicleState::MOVING);
    }
    return granted;
}

bool RideController::moveVehicleToNextBlock(int vehicleIndex) {
    if (vehicleIndex < 0 || vehicleIndex >= static_cast<int>(vehicles.size())) {
        return false;
    }
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
        std::cout << "Vehicle " << vehicle.getVehicleId() 
        << std::endl << "Current Block: " << vehicle.getCurrentBlock() << std::endl
        << std::endl << "Ticks in Current Block: " << vehicle.getTicksInCurrentBlock() << std::endl;
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