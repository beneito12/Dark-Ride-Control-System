#pragma once

enum class VehicleState {
    STOPPED,
    READY_FOR_DISPATCH,
    MOVING,
    FAULTED,
    EMERGENCY_STOP
};

class VehicleContoller
{
    private: 
    int VehicleId;
    int currentBlock;
    VehicleState state;

    public:
    VehicleContoller(int id);
    int getVehicleId() const;
    int getCurrentBlock()const;
    VehicleState getState()const;

    void setCurrentBlock(int blockId);
    void setState(VehicleState newState);
};