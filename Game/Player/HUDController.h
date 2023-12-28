#pragma once
#include <IGUIFont.h>

#include "anax/anax.hpp"

#include "PlayerData.h"

class HUDController
{
public:
    HUDController() = default;

    void init();
    void update(PlayerData &data, bool isInventoryDisplayed) const;
    void destroy();

private:
    irr::video::ITexture
        *m_crosshair, *m_crosshair_interact,
        *m_health_icon_full, *m_health_icon_empty, *m_healthbar_background, *m_healthbar_empty, *m_healthbar_full, *m_ammobackground, *m_water_overlay;

};
