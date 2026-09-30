#pragma once

#include "Block.h"
#include "VehicleController.h"

class RideController {
public:
   private:
   Block block0;
   Block block1;
   Block block2;
   
   VehicleController vehicle1;

   public:
    RideController();

    bool dispatchVehicle();
    bool moveVehicleToNextBlock();
    
    void printStatus() const;
};