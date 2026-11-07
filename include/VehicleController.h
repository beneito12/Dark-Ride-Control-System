#pragma once
#include "CanBus.h"

enum class VehicleState {
    STOPPED,
    READY_FOR_DISPATCH,
    MOVING,
    FAULTED,
    EMERGENCY_STOP,
    WAITING_FOR_DISPATCH,
    COMPLETED_CYCLE
};

class VehicleController
{
    private: 
    int VehicleId;
    int currentBlock;
    VehicleState state;
    int ticksInCurrentBlock;
    bool heartbeatEnabled;

    public:
    VehicleController(int id);
    int getVehicleId() const;
    int getCurrentBlock()const;
    VehicleState getState()const;
    int getTicksInCurrentBlock() const;
    void incrementBlockTimer();

    void setCurrentBlock(int blockId);
    void setState(VehicleState newState);
    
    void sendHeartbeat(CanBus& bus) const;
    void setHeartbeatEnabled(bool enabled);
};