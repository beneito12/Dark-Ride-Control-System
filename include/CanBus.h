#pragma once

#include <vector>
#include "CanMessage.h"

class CanBus
{
    private:
        std::vector<CanMessage> messages;

    public:
        void transmit(const CanMessage& message);

        const std::vector<CanMessage>& getMessages() const;

        void clear();
};