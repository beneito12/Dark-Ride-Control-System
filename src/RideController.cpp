#include "RideController.h"
#include "CanProtocol.h"
#include <iostream>


void RideController::update() {

    canBus.clear(); // Clear old CAN messages before sending new ones
    
    for (VehicleController& vehicle : vehicles) {
        vehicle.sendHeartbeat(canBus);
    }

    for (int i = 0; i < static_cast<int>(vehicles.size()); i++) {
        bool heartbeatReceived = false;

        uint32_t expectedId = VEHICLE_HEARTBEAT_BASE_ID + vehicles[i].getVehicleId();

        for (const CanMessage& message : canBus.getMessages()) {
            if (message.id == expectedId) {
                heartbeatReceived = true;
                break;
            }
        }
        if (heartbeatReceived) {
            missedHeartbeatCounts[i] = 0;
        } else {
            missedHeartbeatCounts[i]++;
            if (missedHeartbeatCounts[i] >= HEARTBEAT_TIMEOUT_TICKS) {
                communicationFaultActive = true;
                vehicles[i].setState(VehicleState::FAULTED);
            }
        }
    }

    if (communicationFaultActive)
    {
        return;
    }
    
    if (station.isEmergencyStopActive()) {
        return;
    }

        // Update vehicles that have completed their cycle to waiting for dispatch
    for (VehicleController& vehicle : vehicles) {
        if (vehicle.getState() == VehicleState::COMPLETED_CYCLE) {
            vehicle.setState(VehicleState::WAITING_FOR_DISPATCH);
        }
    }
    // Update vehicles that are currently in a block and move them to the next block if they have completed the required ticks
    for (int i = 0; i < static_cast<int>(vehicles.size()); i++) {
        int currentBlock = vehicles[i].getCurrentBlock();
        if (currentBlock >= 0 && vehicles[i].getState() == VehicleState::MOVING) {
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

    missedHeartbeatCounts.resize(vehicles.size(), 0);
    communicationFaultActive = false;
}

bool RideController::dispatchVehicle(int vehicleIndex) {

   
    // Attempt to dispatch a vehicle to the first block (station)
    if (vehicleIndex < 0 || vehicleIndex >= static_cast<int>(vehicles.size())) {
        return false;
    }
    VehicleController& vehicle = vehicles[vehicleIndex]; 

    if (!station.isDispatchRequested())
    {
        return false;
    }

    // Consume the operator command immediately.
    // Every dispatch attempt requires a fresh request.
    station.clearDispatchRequest();

    if (!station.canDispatch())
    {
        return false;
    }

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

    std::cout << "Station Status: " << std::endl;
    std::cout << "Gate Closed: " << (station.isGateClosed() ? "YES" : "NO") << std::endl;
    std::cout << "Restraints Locked: " << (station.areRestraintsLocked() ? "YES" : "NO") << std::endl;
    std::cout << "Emergency Stop Active: " << (station.isEmergencyStopActive() ? "YES" : "NO") << std::endl;
    std::cout << "Dispatch Requested: " << (station.isDispatchRequested() ? "YES" : "NO") << std::endl;
    std::cout << "Dispatch Allowed: " << (station.canDispatch() ? "YES" : "NO") << std::endl;

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

void RideController::setStationGateClosed(bool closed) {
    station.setGateClosed(closed);
}

void RideController::setStationRestraintsLocked(bool active) {
    station.setRestraintsLocked(active);
}

void RideController::setStationEmergencyStopActive(bool active) {
    station.setEmergencyStopActive(active);

    if (active)
    {
        station.clearDispatchRequest();

        for (VehicleController& vehicle : vehicles)
        {
            if (vehicle.getCurrentBlock() >= 0)
            {
                vehicle.setState(VehicleState::EMERGENCY_STOP);
            }
        }
    }
}

void RideController::requestStationDispatch() {
    station.requestDispatch();
}

void RideController::resetEmergencyStop() {
    if (station.isEmergencyStopActive()) {
        return;
    }

    for (VehicleController& vehicle : vehicles)
     {
        if (vehicle.getState() == VehicleState::EMERGENCY_STOP)
         {
            vehicle.setState(VehicleState::MOVING);
        }
    }
}

int RideController::getVehicleCurrentBlock(int vehicleIndex) const {
    return vehicles[vehicleIndex].getCurrentBlock();
}

VehicleState RideController::getVehicleState(int vehicleIndex) const {
    return vehicles[vehicleIndex].getState();
}

bool RideController::isBlockOccupied(int blockIndex) const {
    return blocks[blockIndex].isOccupied();
}

int RideController::getBlockOccupyingVehicle(int blockIndex) const {
    return blocks[blockIndex].getOccupyingVehicle();
}

const std::vector<CanMessage>& RideController::getCanMessages() const {
    return canBus.getMessages();
}

void RideController::setVehicleHeartbeatEnabled(int vehicleIndex, bool enabled) 
{
    if (vehicleIndex < 0 || vehicleIndex >= static_cast<int>(vehicles.size())) {
        return;
    }

    vehicles[vehicleIndex].setHeartbeatEnabled(enabled);
}

bool RideController::isCommunicationFaultActive() const {
    return communicationFaultActive;
}

bool RideController::resetCommunicationFault() {
    if (!communicationFaultActive) {
        return false;
    }

    //verify all vehicles communicating
    for (const VehicleController& vehicle : vehicles) {
        uint32_t expectedId = VEHICLE_HEARTBEAT_BASE_ID + vehicle.getVehicleId();

        bool heartbeatReceived = false;
        for (const CanMessage& message : canBus.getMessages()) {
            if (message.id == expectedId) {
                heartbeatReceived = true;
                break;
            }
        }

        if (!heartbeatReceived) {
            return false;
        }
    }

    communicationFaultActive = false;
    
    for (VehicleController& vehicle : vehicles)
    {
        if (vehicle.getState() == VehicleState::FAULTED)
        {
            if (vehicle.getCurrentBlock() >= 0)
            {
                vehicle.setState(VehicleState::MOVING);
            }
            else
            {
                vehicle.setState(VehicleState::WAITING_FOR_DISPATCH);
            }
        }
    }
    return true;
}