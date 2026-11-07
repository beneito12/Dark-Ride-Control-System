#pragma once

#include <cstdint>
#include <array> 

struct CanMessage
{
    uint32_t id;
    std::array<uint8_t, 8> data;
    uint8_t length;
};