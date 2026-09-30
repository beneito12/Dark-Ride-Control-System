#pragma once

#include "Block.h"
#include "VehicleController.h"

class RideController {
public:
   private:
   Block block0;
   VehicleController vehicle1;

   public:
    RideController();

    bool dispatchVehicle();
    void printStatus() const;
};