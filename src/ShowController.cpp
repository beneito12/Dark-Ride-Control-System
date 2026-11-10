#include "ShowController.h"

ShowController::ShowController()
{
    state = ShowState::IDLE;
    showTick = 0;
    activeCue = ShowCue::NONE;
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

    if (showTick == 1)
    {
        activeCue = ShowCue::LIGHTING;
    }
    if (showTick == 2)
    {
        activeCue = ShowCue::ANIMATRONIC;
    }
    if (showTick >= 3) 
    {
        activeCue = ShowCue::AUDIO;
        state = ShowState::COMPLETE;
    }
}

void ShowController::reset()
{
    state = ShowState::IDLE;
    showTick = 0;
    activeCue = ShowCue::NONE;
}

ShowState ShowController::getState() const
{
    return state;
}

int ShowController::getShowTick() const
{
    return showTick;
}

ShowCue ShowController::getActiveCue() const
{
    return activeCue;
}
