#include <catch2/catch_test_macros.hpp>
#include "CanBus.h"
#include "VehicleController.h"

TEST_CASE("CAN bus stores transmitted message")
{
    CanBus bus;

    CanMessage message;
    message.id = 0x101;
    message.length = 2;
    message.data[0] = 1;
    message.data[1] = 3;

    bus.transmit(message);

    const auto& messages = bus.getMessages();

    REQUIRE(messages.size() == 1);
    REQUIRE(messages[0].id == 0x101);
    REQUIRE(messages[0].length == 2);
    REQUIRE(messages[0].data[0] == 1);
    REQUIRE(messages[0].data[1] == 3);
}

TEST_CASE("CAN bus clear removes old messages")
{
    CanBus bus;

    CanMessage message;
    message.id = 0x101;
    message.length = 1;
    message.data[0] = 42;

    bus.transmit(message);

    REQUIRE(bus.getMessages().size() == 1);

    bus.clear();

    REQUIRE(bus.getMessages().empty());
}

TEST_CASE("VehicleController sends heartbeat message")
{
    CanBus bus;
    VehicleController vehicle(1);

    vehicle.sendHeartbeat(bus);

    const auto& messages = bus.getMessages();

    REQUIRE(messages.size() == 1);
    REQUIRE(messages[0].id == 0x101);
    REQUIRE(messages[0].length == 1);
    REQUIRE(messages[0].data[0] == 1);
}