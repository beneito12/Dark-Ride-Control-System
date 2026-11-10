#include "VehicleController.h"
#include "CanProtocol.h"

VehicleController::VehicleController(int id){
    VehicleId = id;
    currentBlock = -1;
    state = VehicleState::WAITING_FOR_DISPATCH;
    ticksInCurrentBlock = 0;
    heartbeatEnabled = true;
}

int VehicleController::getVehicleId() const {
    return VehicleId;
}

int VehicleController::getCurrentBlock() const {
    return currentBlock;
}

VehicleState VehicleController::getState() const {
    return state;
}

void VehicleController::setCurrentBlock(int blockId) {
    currentBlock = blockId;
    ticksInCurrentBlock = 0;
}

void VehicleController::setState(VehicleState newState) {
    state = newState;
}

int VehicleController::getTicksInCurrentBlock() const {
    return ticksInCurrentBlock;
}

void VehicleController::incrementBlockTimer() {
    ticksInCurrentBlock++;
}

void VehicleController::sendHeartbeat(CanBus& bus) const {
    if (!heartbeatEnabled) {
        return;
    }

    CanMessage message{};

    message.id = VEHICLE_HEARTBEAT_BASE_ID + VehicleId;
    message.length = 1;
    message.data[0] = static_cast<uint8_t>(VehicleId);

    bus.transmit(message);
}

void VehicleController::setHeartbeatEnabled(bool enabled)
 {
    heartbeatEnabled = enabled;
}

void VehicleController::sendStatus(CanBus& bus) const {
    CanMessage message{};

    message.id = VEHICLE_STATUS_BASE_ID + VehicleId;
    message.length = 4;

    message.data[0] = static_cast<uint8_t>(VehicleId);
    
    if (currentBlock >= 0) {
        message.data[1] = static_cast<uint8_t>(currentBlock);
    } else {
        message.data[1] = 0xFF; // Indicate no current block
    }
    message.data[2] = static_cast<uint8_t>(state);
    
    uint8_t statusFlags = 0;
    if (state == VehicleState::MOVING){
        statusFlags |= (1 << 0); // Set the MOVING flag
    }
    if (state == VehicleState::FAULTED){
        statusFlags |= (1 << 1); // Set the FAULTED flag
    }
    if (state == VehicleState::EMERGENCY_STOP){
        statusFlags |= (1 << 2); // Set the EMERGENCY_STOP flag
    } 
    message.data[3] = statusFlags;  

    bus.transmit(message);
}