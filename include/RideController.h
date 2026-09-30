#pragma once

#include "Block.h"
#include "VehicleController.h"
#include <vector>

class RideController {
public:
   private:
   std::vector<Block> blocks;
   
   VehicleController vehicle1;

   public:
    RideController();

    bool dispatchVehicle();
    bool moveVehicleToNextBlock();

    void printStatus() const;
};