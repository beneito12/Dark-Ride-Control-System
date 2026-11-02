#include "VehicleController.h"

VehicleController::VehicleController(int id){
    VehicleId = id;
    currentBlock = -1;
    state = VehicleState::WAITING_FOR_DISPATCH;
    ticksInCurrentBlock = 0;
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