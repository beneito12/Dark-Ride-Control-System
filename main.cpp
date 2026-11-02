#include <iostream>
#include "RideController.h"


int main() {
   std::cout << "Virtual Attraction Control System\n\n";
   RideController ride;
   ride.setStationGateClosed(true);
   ride.setStationRestraintsLocked(true);
   ride.setStationEmergencyStopActive(false);

   ride.requestStationDispatch();
    
   for (int step = 0; step < 10; step++) {
    std::cout << "\n-- Simulation Step " << step << " --\n";
    ride.update();
    ride.printStatus();
   }
}