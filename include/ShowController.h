#pragma once

enum class ShowState
{
    IDLE,
    RUNNING,
    COMPLETE
};

class ShowController
{
    private:
    ShowState state;
    int showTick;

    public: 
        ShowController();

        void startShow();
        void update();
        void reset();

        ShowState getState() const;
        int getShowTick() const;
};