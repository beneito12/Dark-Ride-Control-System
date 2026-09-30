#include <iostream>
#include "Block.h"

int main() {
    std::cout << "Virtual Attraction Control System\n\n";

    Block block0(0);

    std::cout << "Block " << block0.getId() << " occupied? " << 
    (block0.isOccupied() ? "Yes" : "No") << std::endl;

    bool granted = block0.requestEntry(1);
    std::cout << "Vehicle 1 entry granted? " << (granted ? "Yes" : "No") << std::endl;

    std::cout << "Block " << block0.getId() << " occupied? " << 
    (block0.isOccupied() ? "Yes" : "No") << std::endl;
    return 0;
}