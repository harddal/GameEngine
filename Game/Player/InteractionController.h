#pragma once

#include "PlayerData.h"

// NOIMP
class InteractionController
{
public:
    InteractionController() = default;

    void init();
    void update(PlayerData &data);
    void destroy();

private:

};
