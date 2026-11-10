#include "ShowController.h"

ShowController::ShowController()
{
    state = ShowState::IDLE;
    showTick = 0;
}

void ShowController::startShow()
{
    if (state == ShowState::IDLE)
    {
        state = ShowState::RUNNING;
        showTick = 0;
    }
}

void ShowController::update()
{
    if (state != ShowState::RUNNING)
    {
        return;
    }
    showTick++;
    if (showTick >= 3) // Example condition for show completion
    {
        state = ShowState::COMPLETE;
    }
}

void ShowController::reset()
{
    state = ShowState::IDLE;
    showTick = 0;
}

ShowState ShowController::getState() const
{
    return state;
}

int ShowController::getShowTick() const
{
    return showTick;
}
