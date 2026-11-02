#pragma once

class StationController
{
    private:
        bool gateClosed;
        bool restraintsLocked;
        bool emergencyStopActive;

    public:
        StationController();

        bool isGateClosed() const;
        bool areRestraintsLocked() const;
        bool isEmergencyStopActive() const;

        void setGateClosed(bool closed);
        void setRestraintsLocked(bool locked);
        void setEmergencyStopActive(bool active);

        bool canDispatch() const;

};