#pragma once

enum class ShowState
{
    IDLE,
    RUNNING,
    COMPLETE
};

enum class ShowCue
{
    NONE,
    LIGHTING,
    ANIMATRONIC,
    AUDIO
};

class ShowController
{
    private:
    ShowState state;
    int showTick;
    ShowCue activeCue;

    public: 
        ShowController();

        void startShow();
        void update();
        void reset();

        ShowState getState() const;
        int getShowTick() const;
        ShowCue getActiveCue() const;
};