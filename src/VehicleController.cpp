#include "VehicleController.h"

VehicleController::VehicleController(int id){
    VehicleId = id;
    currentBlock = -1;
    state = VehicleState::STOPPED;
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
}

void VehicleController::setState(VehicleState newState) {
    state = newState;
}