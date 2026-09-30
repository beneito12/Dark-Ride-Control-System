#include "Block.h"

Block::Block(int blockId, int dwellTicks)
{
    id = blockId;
    requiredTicks = dwellTicks;
    occupyingvehicle = -1;
}

int Block::getRequiredTicks() const
{
    return requiredTicks;
}

bool Block::isOccupied() const
{
    return occupyingvehicle != -1;
}

bool Block::requestEntry(int vehicleId)
{
    if (!isOccupied())
    {
        occupyingvehicle = vehicleId;
        return true;
    }
    return false;
}

void Block::release()
{
    occupyingvehicle = -1;
}

int Block::getId() const
{
    return id;
}

int Block::getOccupyingVehicle() const
{
    return occupyingvehicle;
}