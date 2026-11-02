#pragma once

#include "Block.h"
#include "VehicleController.h"
#include <vector>
#include "StationController.h"

class RideController {
public:
   private:
   std::vector<Block> blocks;
   
   std::vector<VehicleController> vehicles;
   void completeRideCycle(int vehicleIndex);

   StationController station;

  

   public:
    RideController();

    int getVehicleId(int vehicleIndex) const;
    void update();

    bool dispatchVehicle(int vehicleIndex);
    bool moveVehicleToNextBlock(int vehicleIndex);

    void printStatus() const;

    void setStationGateClosed(bool closed);
    void setStationRestraintsLocked(bool locked);
    void setStationEmergencyStopActive(bool active);
};