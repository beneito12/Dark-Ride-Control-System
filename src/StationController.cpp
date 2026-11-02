#include "StationController.h"

StationController::StationController()
{
    gateClosed = false;
    restraintsLocked = false;
    emergencyStopActive = false;
}

bool StationController::isGateClosed() const
{
    return gateClosed;
}

bool StationController::areRestraintsLocked() const
{
    return restraintsLocked;
}

bool StationController::isEmergencyStopActive() const
{
    return emergencyStopActive;
}

void StationController::setGateClosed(bool closed)
{
    gateClosed = closed;
}

void StationController::setRestraintsLocked(bool locked)
{
    restraintsLocked = locked;
}

void StationController::setEmergencyStopActive(bool active)
{
    emergencyStopActive = active;
}

bool StationController::canDispatch() const
{
    return gateClosed && restraintsLocked && !emergencyStopActive;
}