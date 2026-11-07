#include "CanBus.h"

void CanBus::transmit(const CanMessage& message)
{
    messages.push_back(message);
}

const std::vector<CanMessage>& CanBus::getMessages() const
{
    return messages;
}

void CanBus::clear()
{
    messages.clear();
}