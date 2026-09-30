#include <iostream>
#include "RideController.h"

void dispatchAndReport(RideController& ride, int vehicleIndex) {
    bool dispatched = ride.dispatchVehicle(vehicleIndex);
    std::cout << "Vehicle " << ride.getVehicleId(vehicleIndex) << " dispatch request: " 
              << (dispatched ? "Granted" : "Denied") << std::endl;
}

 void moveAndReport(RideController& ride, int vehicleIndex) {
    bool moved = ride.moveVehicleToNextBlock(vehicleIndex);
    std::cout << "Vehicle " << ride.getVehicleId(vehicleIndex) << " move request: " 
              << (moved ? "Successful" : "Failed") << std::endl;
}

int main() {
    std::cout << "Virtual Attraction Control System\n\n";
    RideController ride;
    dispatchAndReport(ride, 0);
    moveAndReport(ride, 0);
    
    dispatchAndReport(ride, 1);
    moveAndReport(ride, 1);
    
    return 0;
}