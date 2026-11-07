#include <catch2/catch_test_macros.hpp>
#include "StationController.h"

TEST_CASE("Gate Starts Open") {
    StationController station;
    REQUIRE_FALSE(station.isGateClosed());
}

TEST_CASE("Gate Can Be Closed") {
    StationController station;
    station.setGateClosed(true);
    REQUIRE(station.isGateClosed());
}

TEST_CASE("Gate Can Be Opened") {
    StationController station;
    station.setGateClosed(true);
    station.setGateClosed(false);
    REQUIRE_FALSE(station.isGateClosed());
}

TEST_CASE("Restraints Start Unlocked") {
    StationController station;
    REQUIRE_FALSE(station.areRestraintsLocked());
}

TEST_CASE("Restraints Can Be Locked") {
    StationController station;
    station.setRestraintsLocked(true);
    REQUIRE(station.areRestraintsLocked());
}

TEST_CASE("Restraints Can Be Unlocked") {
    StationController station;
    station.setRestraintsLocked(true);
    station.setRestraintsLocked(false);
    REQUIRE_FALSE(station.areRestraintsLocked());
}

TEST_CASE("Emergency Stop Starts Inactive") {
    StationController station;
    REQUIRE_FALSE(station.isEmergencyStopActive());
}

TEST_CASE("Emergency Stop Can Be Activated") {
    StationController station;
    station.setEmergencyStopActive(true);
    REQUIRE(station.isEmergencyStopActive());
}

TEST_CASE("Emergency Stop Can Be Deactivated") {
    StationController station;
    station.setEmergencyStopActive(true);
    station.setEmergencyStopActive(false);
    REQUIRE_FALSE(station.isEmergencyStopActive());
}

TEST_CASE("Dispatch Starts Not Requested") {
    StationController station;
    REQUIRE_FALSE(station.isDispatchRequested());
}

TEST_CASE("Dispatch Can Be Requested") {
    StationController station;
    station.requestDispatch();
    REQUIRE(station.isDispatchRequested());
}

TEST_CASE("Dispatch Can Be Cleared") {
    StationController station;
    station.requestDispatch();
    station.clearDispatchRequest();
    REQUIRE_FALSE(station.isDispatchRequested());
}

TEST_CASE("Dispatch Can Be Allowed")
{
    StationController station;

    station.setGateClosed(true);
    station.setRestraintsLocked(true);
    station.setEmergencyStopActive(false);

    REQUIRE(station.canDispatch());
}


TEST_CASE("Dispatch Cannot Be Allowed When Gate Is Open")
{
    StationController station;

    station.setGateClosed(false);
    station.setRestraintsLocked(true);
    station.setEmergencyStopActive(false);

    REQUIRE_FALSE(station.canDispatch());
}

TEST_CASE("Dispatch Cannot Be Allowed When Restraints Are Unlocked")
{
    StationController station;

    station.setGateClosed(true);
    station.setRestraintsLocked(false);
    station.setEmergencyStopActive(false);

    REQUIRE_FALSE(station.canDispatch());
}

TEST_CASE("Dispatch Cannot Be Allowed When Emergency Stop Is Active") {
    StationController station;
    station.setEmergencyStopActive(true);
    station.requestDispatch();
    REQUIRE_FALSE(station.canDispatch());
}   
