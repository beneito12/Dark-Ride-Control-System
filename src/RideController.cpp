#include "RideController.h"
#include "CanProtocol.h"
#include <iostream>


void RideController::update() {

    currentTick++;

    canBus.clear(); // Clear old CAN messages before sending new ones
    
    for (VehicleController& vehicle : vehicles) {
        vehicle.sendHeartbeat(canBus);
        vehicle.sendStatus(canBus);
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
                if (!communicationFaultActive) {
                     communicationFaultActive = true;
                communicationFaultVehicleId = vehicles[i].getVehicleId();

                    logEvent("Communication fault detected for vehicle " + std::to_string(vehicles[i].getVehicleId()));
                }

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

    showController.update();

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

RideController::RideController(int vehicleCount)
{
    blocks.emplace_back(0, 2); // station
    blocks.emplace_back(1, 1); // track
    blocks.emplace_back(2, 3); // show scene

    for (int id = 1; id <= vehicleCount; id++)
    {
        vehicles.emplace_back(id);
    }

    missedHeartbeatCounts.resize(vehicles.size(), 0);
    communicationFaultActive = false;
    communicationFaultVehicleId = -1;
    currentTick = 0;
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
    if (granted)
{
    blocks[currentBlock].release();
    vehicle.setCurrentBlock(nextBlock);

    if (nextBlock == 2)
    {
        CanMessage showTrigger{};

        showTrigger.id = SHOW_TRIGGER_ID;
        showTrigger.length = 1;
        showTrigger.data[0] =
            static_cast<uint8_t>(vehicle.getVehicleId());

        canBus.transmit(showTrigger);

        showController.startShow();

        logEvent("Vehicle " + std::to_string(vehicle.getVehicleId()) + " entered Show Block - show triggered");
    }
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
        logEvent("Emergency stop activated");
        station.clearDispatchRequest();

        for (VehicleController& vehicle : vehicles)
        {
            if (vehicle.getCurrentBlock() >= 0)
            {
                vehicle.setState(VehicleState::EMERGENCY_STOP);
            }
        }
    }
    else{
        logEvent("Emergency stop cleared");
    }
}

void RideController::requestStationDispatch() {
    station.requestDispatch();
}

void RideController::resetEmergencyStop() {
    if (station.isEmergencyStopActive()) {
        return;
    }

    bool resetperformed = false;
    
    for (VehicleController& vehicle : vehicles)
     {
        if (vehicle.getState() == VehicleState::EMERGENCY_STOP)
         {
            vehicle.setState(VehicleState::MOVING);
            resetperformed = true;
        }
    }
    
    if (resetperformed) {
        logEvent("Emergency stop reset accepted");
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

            logEvent("Communication fault reset failed: Vehicle " + std::to_string(vehicle.getVehicleId()) + " heartbeat missing");
            return false;
        }
    }

    communicationFaultActive = false;

    communicationFaultVehicleId = -1;

    for (int& count : missedHeartbeatCounts)
    {
        count = 0;
    }
    
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

    logEvent("Communication fault reset accepted");
    return true;
}

ShowState RideController::getShowState() const {
    return showController.getState();
}

int RideController::getShowTick() const {
    return showController.getShowTick();
}

ShowCue RideController::getActiveShowCue() const {
    return showController.getActiveCue();
}

int RideController::getVehicleCount() const {
    return static_cast<int>(vehicles.size());
}

int RideController::getCommunicationFaultVehicleId() const {
    return communicationFaultVehicleId;
}

int RideController::getMissedHeartbeatCount(int vehicleIndex) const {
    if (vehicleIndex < 0 || vehicleIndex >= static_cast<int>(missedHeartbeatCounts.size())) {
        return -1;
    }
    return missedHeartbeatCounts[vehicleIndex];
}

void RideController::logEvent(const std::string& message) {
    eventLog.push_back("[Tick " + std::to_string(currentTick) + "] " + message);
}

const std::vector<std::string>& RideController::getEventLog() const {
    return eventLog;
}

void RideController::clearEventLog() {
    eventLog.clear();
}