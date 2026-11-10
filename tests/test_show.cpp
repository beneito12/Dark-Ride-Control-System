#include <catch2/catch_test_macros.hpp>
#include "ShowController.h"

TEST_CASE("Show controller starts idle")
{
    ShowController show;

    REQUIRE(show.getState() == ShowState::IDLE);
    REQUIRE(show.getShowTick() == 0);
}

TEST_CASE("Show controller runs and completes sequence")
{
    ShowController show;

    show.startShow();

    REQUIRE(show.getState() == ShowState::RUNNING);

    show.update();
    REQUIRE(show.getShowTick() == 1);

    show.update();
    REQUIRE(show.getShowTick() == 2);

    show.update();

    REQUIRE(show.getShowTick() == 3);
    REQUIRE(show.getState() == ShowState::COMPLETE);
}

TEST_CASE("Show controller reset returns to idle")
{
    ShowController show;

    show.startShow();
    show.update();

    show.reset();

    REQUIRE(show.getState() == ShowState::IDLE);
    REQUIRE(show.getShowTick() == 0);
}

TEST_CASE("Show controller advances through show cues")
{
    ShowController show;

    show.startShow();

    REQUIRE(show.getActiveCue() == ShowCue::NONE);

    show.update();
    REQUIRE(show.getActiveCue() == ShowCue::LIGHTING);

    show.update();
    REQUIRE(show.getActiveCue() == ShowCue::ANIMATRONIC);

    show.update();
    REQUIRE(show.getActiveCue() == ShowCue::AUDIO);
    REQUIRE(show.getState() == ShowState::COMPLETE);
}