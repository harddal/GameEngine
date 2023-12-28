#include "HUDController.h"

#include "Engine/Engine.h"
#include "Game/Item/ItemDatabase.h"

#include "Game/Components.h"

#include "PlayerController.h"

#define _crosshair_center_position irr::core::vector2d<irr::s32>((RenderManager::Get()->getConfiguration().width / 2) - (m_crosshair->getSize().Width / 2), (RenderManager::Get()->getConfiguration().height / 2) - (m_crosshair->getSize().Height / 2))

void HUDController::init()
{
	m_crosshair = RenderManager::Get()->driver()->getTexture("content/texture/ui/cross.png");
	if (!m_crosshair) {
		spdlog::error("Failed to load texture asset: m_crosshair");
	}

	m_crosshair_interact = RenderManager::Get()->driver()->getTexture("content/texture/ui/cross-interact.png");
	if (!m_crosshair_interact) {
		spdlog::error("Failed to load texture asset: m_crosshair_interact");
	}

	m_healthbar_background = RenderManager::Get()->driver()->getTexture("content/texture/ui/healthbar_background.png");
	if (!m_healthbar_background) {
		spdlog::error("Failed to load texture asset: m_healthbar_background");
	}

	m_health_icon_empty = RenderManager::Get()->driver()->getTexture("content/texture/ui/health_icon_empty.png");
	if (!m_health_icon_empty) {
		spdlog::error("Failed to load texture asset: m_health_icon_empty");
	}

	m_health_icon_full = RenderManager::Get()->driver()->getTexture("content/texture/ui/health_icon_full.png");
	if (!m_health_icon_full) {
		spdlog::error("Failed to load texture asset: m_health_icon_full");
	}

	m_healthbar_empty = RenderManager::Get()->driver()->getTexture("content/texture/ui/healthbar_empty.png");
	if (!m_healthbar_empty) {
		spdlog::error("Failed to load texture asset: m_healthbar_empty");
	}

	m_healthbar_full = RenderManager::Get()->driver()->getTexture("content/texture/ui/healthbar_full.png");
	if (!m_healthbar_full) {
		spdlog::error("Failed to load texture asset: m_healthbar_full");
	}

	m_ammobackground = RenderManager::Get()->driver()->getTexture("content/texture/ui/ammo_background.png");
	if (!m_ammobackground) {
		spdlog::error("Failed to load texture asset: m_ammobackground");
	}

	m_water_overlay = RenderManager::Get()->driver()->getTexture("content/texture/ui/water_overlay.png");
	if (!m_water_overlay) {
		spdlog::error("Failed to load texture asset: m_water_overlay");
	}
}

void HUDController::update(PlayerData &data, bool isInventoryDisplayed) const
{
	auto &player = WorldManager::Get()->managerSystem()->getEntityByName("player");
	if (player.isValid()) {

		// --- WATER ---
		if (g_PlayerController->isHeadUnderWater())
		{
			RenderManager::Get()->renderImage2D(
				m_water_overlay,
				irr::core::vector2di(0, 0));
		}

		// --- CROSSHAIR ---
		if (!isInventoryDisplayed) {
			RenderManager::Get()->renderImage2D(
				m_crosshair,
				_crosshair_center_position);

			auto hit = RenderManager::Get()->raycastWorldPosition(
				player.getComponent<CameraComponent>().camera->getAbsolutePosition(),
				player.getComponent<CameraComponent>().targetNode->getAbsolutePosition(),
				true);

			if (hit.node) {
				auto target = WorldManager::Get()->managerSystem()->getEntityByID(hit.node->getID());
				if (target.isValid()) {
					if ((target.hasComponent<InteractionComponent>() || target.hasComponent<ItemComponent>()) &&
						Math::Stable_3D_Distance(player.getComponent<CameraComponent>().camera->getAbsolutePosition(), hit.point) < _player_interact_distance) {
						RenderManager::Get()->renderImage2D(
							m_crosshair_interact,
							_crosshair_center_position,
							irr::video::SColor(255, 51, 51, 255));

						if (target.hasComponent<ItemComponent>()) {
							auto item = ItemDatabase::GetItemByName(target.getComponent<ItemComponent>().item);
							auto value = irr::core::stringw(L"Pickup ");
							value += irr::core::stringw(item.name.c_str());

							RenderManager::Get()->renderText2D(
								value,
								TEXT_DEFAULT_FONT::SMALL,
								irr::core::rect<irr::s32>((_crosshair_center_position.X - value.size() * 4 / 2) - 8,
									_crosshair_center_position.Y + 48, 0, 0));
						}
						else {
							auto value = irr::core::stringw(L"Interact");

							RenderManager::Get()->renderText2D(
								value,
								TEXT_DEFAULT_FONT::SMALL,
								irr::core::rect<irr::s32>(
									_crosshair_center_position.X - value.size() * 4 / 2,
									_crosshair_center_position.Y + 48, 0, 0));
						}
					}
					else if (target.hasComponent<DamageReceiverComponent>()) {
						RenderManager::Get()->renderImage2D(
							m_crosshair_interact,
							_crosshair_center_position,
							irr::video::SColor(255, 255, 51, 51));
					}
				}
			}
		}

		// --- HEALTH ---
#define health_icon_position irr::core::vector2di(0, RenderManager::Get()->getConfiguration().height - m_health_icon_full->getSize().Height)
#define health_background_position irr::core::vector2di(m_health_icon_full->getSize().Width - 9, RenderManager::Get()->getConfiguration().height - m_healthbar_background->getSize().Height)
#define health_pip_full_position(n) irr::core::vector2di(m_health_icon_full->getSize().Width - 9 + 5 + n * 25, RenderManager::Get()->getConfiguration().height - 4 - m_healthbar_full->getSize().Height)
#define health_pip_empty_position(n) irr::core::vector2di(m_health_icon_full->getSize().Width - 9 + 5 + n * 25, RenderManager::Get()->getConfiguration().height - 6 - m_healthbar_full->getSize().Height)

		irr::video::SColor healthbar_color;

		if (data.currentHealth > 0) {
			healthbar_color = irr::video::SColor(255, 251, 105, 98);
		}
		if (data.currentHealth > 24) {
			healthbar_color = irr::video::SColor(255, 252, 252, 153);
		}
		if (data.currentHealth > 51) {
			healthbar_color = irr::video::SColor(255, 121, 222, 121);
		}

		if (data.currentHealth < 25) {
			double current_time = Engine::Get()->getCurrentTime();
			static double last_time = 0.0;
			static bool display_empty = true;

			if (current_time - last_time > 500.0) {
				display_empty = !display_empty;

				last_time = current_time;
			}

			if (display_empty) {
				RenderManager::Get()->renderImage2D(
					m_health_icon_empty,
					health_icon_position, healthbar_color);
			}
			else {
				RenderManager::Get()->renderImage2D(
					m_health_icon_full,
					health_icon_position, healthbar_color);
			}
		}
		else {
			RenderManager::Get()->renderImage2D(
				m_health_icon_full,
				health_icon_position, healthbar_color);
		}

		RenderManager::Get()->renderImage2D(
			m_healthbar_background,
			health_background_position);

		if (data.currentHealth > 0) {
			RenderManager::Get()->renderImage2D(
				m_healthbar_full,
				health_pip_full_position(0), healthbar_color);
		}
		else {
			RenderManager::Get()->renderImage2D(
				m_healthbar_empty,
				health_pip_empty_position(0), healthbar_color);
		}
		if (data.currentHealth > 10) {
			RenderManager::Get()->renderImage2D(
				m_healthbar_full,
				health_pip_full_position(1), healthbar_color);
		}
		else {
			RenderManager::Get()->renderImage2D(
				m_healthbar_empty,
				health_pip_empty_position(1), healthbar_color);
		}
		if (data.currentHealth > 20) {
			RenderManager::Get()->renderImage2D(
				m_healthbar_full,
				health_pip_full_position(2), healthbar_color);
		}
		else {
			RenderManager::Get()->renderImage2D(
				m_healthbar_empty,
				health_pip_empty_position(2), healthbar_color);
		}
		if (data.currentHealth > 30) {
			RenderManager::Get()->renderImage2D(
				m_healthbar_full,
				health_pip_full_position(3), healthbar_color);
		}
		else {
			RenderManager::Get()->renderImage2D(
				m_healthbar_empty,
				health_pip_empty_position(3), healthbar_color);
		}
		if (data.currentHealth > 40) {
			RenderManager::Get()->renderImage2D(
				m_healthbar_full,
				health_pip_full_position(4), healthbar_color);
		}
		else {
			RenderManager::Get()->renderImage2D(
				m_healthbar_empty,
				health_pip_empty_position(4), healthbar_color);
		}
		if (data.currentHealth > 50) {
			RenderManager::Get()->renderImage2D(
				m_healthbar_full,
				health_pip_full_position(5), healthbar_color);
		}
		else {
			RenderManager::Get()->renderImage2D(
				m_healthbar_empty,
				health_pip_empty_position(5), healthbar_color);
		}
		if (data.currentHealth > 60) {
			RenderManager::Get()->renderImage2D(
				m_healthbar_full,
				health_pip_full_position(6), healthbar_color);
		}
		else {
			RenderManager::Get()->renderImage2D(
				m_healthbar_empty,
				health_pip_empty_position(6), healthbar_color);
		}
		if (data.currentHealth > 70) {
			RenderManager::Get()->renderImage2D(
				m_healthbar_full,
				health_pip_full_position(7), healthbar_color);
		}
		else {
			RenderManager::Get()->renderImage2D(
				m_healthbar_empty,
				health_pip_empty_position(7), healthbar_color);
		}
		if (data.currentHealth > 80) {
			RenderManager::Get()->renderImage2D(
				m_healthbar_full,
				health_pip_full_position(8), healthbar_color);
		}
		else {
			RenderManager::Get()->renderImage2D(
				m_healthbar_empty,
				health_pip_empty_position(8), healthbar_color);
		}
		if (data.currentHealth > 90) {
			RenderManager::Get()->renderImage2D(
				m_healthbar_full,
				health_pip_full_position(9), healthbar_color);
		}
		else {
			RenderManager::Get()->renderImage2D(
				m_healthbar_empty,
				health_pip_empty_position(9), healthbar_color);
		}

		// --- AMMO ---
		if (data.isWeaponEquipped && data.ammoDisplayValue >= 0) {
			RenderManager::Get()->renderImage2D(
				m_ammobackground,
				irr::core::vector2di(RenderManager::Get()->getConfiguration().width - m_ammobackground->getSize().Width, RenderManager::Get()->getConfiguration().height - m_ammobackground->getSize().Height));

			auto value = irr::core::stringw(L"Ammo: ");
			value += data.ammoDisplayValue;
			RenderManager::Get()->renderText2D(
				value,
				TEXT_DEFAULT_FONT::SMALL,
				irr::core::rect<irr::s32>(RenderManager::Get()->getConfiguration().width - (m_ammobackground->getSize().Width / 2) + 20 - value.size() * 8, RenderManager::Get()->getConfiguration().height + 30 - m_ammobackground->getSize().Height - 10, 0, 0),
				irr::video::SColor(255, 255, 255, 255));
		}
	}
}

void HUDController::destroy()
{

}
