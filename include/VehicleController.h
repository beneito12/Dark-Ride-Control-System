#pragma once

enum class VehicleState {
    STOPPED,
    READY_FOR_DISPATCH,
    MOVING,
    FAULTED,
    EMERGENCY_STOP
};

class VehicleController
{
    private: 
    int VehicleId;
    int currentBlock;
    VehicleState state;
     int ticksInCurrentBlock;

    public:
    VehicleController(int id);
    int getVehicleId() const;
    int getCurrentBlock()const;
    VehicleState getState()const;
    int getTicksInCurrentBlock() const;
    void incrementBlockTimer();

    void setCurrentBlock(int blockId);
    void setState(VehicleState newState);
};