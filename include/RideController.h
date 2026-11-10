#pragma once

#include "Block.h"
#include "VehicleController.h"
#include <vector>
#include "StationController.h"
#include "CanBus.h"
#include "ShowController.h"

class RideController {
   private:
   std::vector<Block> blocks;
   
   std::vector<VehicleController> vehicles;
   void completeRideCycle(int vehicleIndex);

   StationController station;

   CanBus canBus;
   ShowController showController;
  
    std::vector<int> missedHeartbeatCounts;
    bool communicationFaultActive;
    static constexpr int HEARTBEAT_TIMEOUT_TICKS = 3;

   public:
    RideController();

    int getVehicleId(int vehicleIndex) const;
    void update();
    void resetEmergencyStop();

    bool dispatchVehicle(int vehicleIndex);
    bool moveVehicleToNextBlock(int vehicleIndex);

    void printStatus() const;

    void setStationGateClosed(bool closed);
    void setStationRestraintsLocked(bool locked);
    void setStationEmergencyStopActive(bool active);

    void requestStationDispatch();
    
    int getVehicleCurrentBlock(int vehicleIndex) const;
    VehicleState getVehicleState(int vehicleIndex) const;
    bool isBlockOccupied(int blockIndex) const;
    int getBlockOccupyingVehicle(int blockIndex) const;

    const std::vector<CanMessage>& getCanMessages() const;

    void setVehicleHeartbeatEnabled(int vehicleIndex, bool enabled);
    bool isCommunicationFaultActive() const;

    bool resetCommunicationFault();

    ShowState getShowState() const;
    int getShowTick() const;

    ShowCue getActiveShowCue() const;
};