#pragma once

class Block
{
    private:
    int id;
    int occupyingvehicle;

    public:
    Block(int blockId);

    bool isOccupied() const;
    bool requestEntry(int vehicleId);
    void release();

    int getId() const;
    int getOccupyingVehicle() const;
};