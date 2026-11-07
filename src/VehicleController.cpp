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
