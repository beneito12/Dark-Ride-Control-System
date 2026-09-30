#pragma once

class Block
{
    private:
    int id;
    int occupyingvehicle;
    int requiredTicks;

    public:
    Block(int blockId, int dwellTicks);
    int getRequiredTicks() const;

    bool isOccupied() const;
    bool requestEntry(int vehicleId);
    void release();

    int getId() const;
    int getOccupyingVehicle() const;
};