#pragma once

#include "Block.h"
#include "VehicleController.h"
#include <vector>
#include "StationController.h"
#include "CanBus.h"

class RideController {
public:
   private:
   std::vector<Block> blocks;
   
   std::vector<VehicleController> vehicles;
   void completeRideCycle(int vehicleIndex);

   StationController station;

   CanBus canBus;

  

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
};