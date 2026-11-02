#include "RideController.h"
#include <iostream>

void RideController::update() {

        // Update vehicles that have completed their cycle to waiting for dispatch
    for (VehicleController& vehicle : vehicles) {
        if (vehicle.getState() == VehicleState::COMPLETED_CYCLE) {
            vehicle.setState(VehicleState::WAITING_FOR_DISPATCH);
        }
    }
    // Update vehicles that are currently in a block and move them to the next block if they have completed the required ticks
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
    // Dispatch vehicles that are waiting for dispatch and have no current block
    for (int i = 0; i < static_cast<int>(vehicles.size()); i++) {
        if (vehicles[i].getCurrentBlock() == -1 && vehicles[i].getState() == VehicleState::WAITING_FOR_DISPATCH) {
            dispatchVehicle(i);
        }
    }
}

RideController::RideController() {
    // Initialize the ride with blocks and vehicles
    blocks.emplace_back(0, 2); // station
    blocks.emplace_back(1, 1); // track
    blocks.emplace_back(2, 3); // show scene
    vehicles.emplace_back(1);
    vehicles.emplace_back(2);
}

bool RideController::dispatchVehicle(int vehicleIndex) {
    // Attempt to dispatch a vehicle to the first block (station)
    if (vehicleIndex < 0 || vehicleIndex >= static_cast<int>(vehicles.size())) {
        return false;
    }
    VehicleController& vehicle = vehicles[vehicleIndex]; 

    // Request entry to the first block (station) for the vehicle
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
     // Check if the vehicle is currently in a valid block before attempting to move it to the next block
    if (currentBlock < 0) return false;

    if (nextBlock >= static_cast<int>(blocks.size())) { // Check if the next block is beyond the last block, indicating the end of the ride cycle
        completeRideCycle(vehicleIndex);
        return true;
    }
    bool granted = blocks[nextBlock].requestEntry(vehicle.getVehicleId()); // Request entry to the next block for the vehicle
    if (granted) {
        blocks[currentBlock].release(); // Release the current block for the vehicle
        vehicle.setCurrentBlock(nextBlock); // Update the vehicle's current block to the next block
    }
    return granted;
}

void RideController::printStatus() const { // Print the current status of all vehicles and blocks in the ride
    std::cout << "Ride Status: " << std::endl;

    for (const VehicleController& vehicle : vehicles) { // Print the status of each vehicle
        std::cout << "Vehicle " << vehicle.getVehicleId() 
        << std::endl << "Current Block: " << vehicle.getCurrentBlock() << std::endl
        << std::endl << "Ticks in Current Block: " << vehicle.getTicksInCurrentBlock() << std::endl;
    }

    for (const Block& block : blocks) { // Print the status of each block
        std::cout << "Block " << block.getId() << ": "; 
        if (block.isOccupied()) {
            std::cout << "OCCUPIED" << std::endl;
            std::cout << " by vehicle " << block.getOccupyingVehicle() << std::endl;
        } else {
            std::cout << "CLEAR" << std::endl;
        }
    }
}

int RideController::getVehicleId(int vehicleIndex) const { // Get the ID of a vehicle by its index
    return vehicles[vehicleIndex].getVehicleId();
}

void RideController::completeRideCycle(int vehicleIndex) { // Handle the completion of a vehicle's ride cycle
    VehicleController& vehicle = vehicles[vehicleIndex];

    int currentBlock = vehicle.getCurrentBlock(); // Get the current block of the vehicle
    if (currentBlock >= 0) blocks[currentBlock].release(); // Release the current block for the vehicle

    vehicle.setCurrentBlock(-1); // Reset the vehicle's current block to indicate it is not in any block
    vehicle.setState(VehicleState::COMPLETED_CYCLE); // Update the vehicle's state to indicate the ride cycle is completed
}