#include <iostream>
#include "RideController.h"

int main() {
    std::cout << "Virtual Attraction Control System\n\n";

    RideController ride;

    bool dispatched = ride.dispatchVehicle();

    std::cout << "Dispatch request: " << (dispatched ? "Granted" : "Denied") << std::endl;

    ride.printStatus();

    return 0;
}