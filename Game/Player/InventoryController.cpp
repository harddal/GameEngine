#include "InventoryController.h"

#include "Game/Components.h"

#include "Game/Item/ItemDatabase.h"
#include "Engine/Resource/FilePaths.h"
#include "PlayerController.h"
#include <IMGUI/imgui_internal.h>

using namespace ImGui;

#undef MB_RIGHT

#define _inventory_data g_PlayerData.inventoryData

#define _inventory_y_offset 8
#define _slot_size_px 64
#define _slot_spacing 1
#define _slot_half_px (_slot_size_px / 32)
#define _slot_spacing_px (_slot_size_px + 1)
#define _current_gridslot (x + _inventory_data.size.X * y)

#define _slot_position_x (x * _slot_spacing_px + ((RenderManager::Get()->getConfiguration().width / 2)  - _inventory_data.size.X * (_slot_size_px / 2)))
#define _slot_position_y ((y * _slot_spacing_px) + _inventory_y_offset) 
#define _slot_position_vec2 irr::core::vector2di(_slot_position_x, _slot_position_y)

#define _mouse_over_slot (mouse_position.X > _slot_position_x - 1 && mouse_position.X < _slot_position_x + 65 && mouse_position.Y > _slot_position_y - 1 && mouse_position.Y < _slot_position_y + 65)

#define _equip_slot_2h_size_x 256
#define _equip_slot_2h_size_y 64
#define _equip_slot_position_2h (irr::core::vector2di((RenderManager::Get()->getConfiguration().width / 2)  - (_equip_slot_2h_size_x / 2), _inventory_y_offset + (_slot_size_px * 3) + 3))

#define _mouse_over_equip_slot_2h (mouse_position.X > _equip_slot_position_2h.X && mouse_position.X < _equip_slot_position_2h.X + _equip_slot_2h_size_x && mouse_position.Y > _equip_slot_position_2h.Y + (_slot_spacing * 4) && mouse_position.Y < _equip_slot_position_2h.Y + (_slot_spacing * 4) + _equip_slot_2h_size_y)

#define ITEM_NAME_TOOLTIP_OFFSET_Y 20

bool g_PlayerInventoryIsDisplaying, g_LockPlayerForInput;

void SetIMGUI_PlayerInventoryTheme()
{
    ImGui::PushStyleColor(ImGuiCol_Text,             ImVec4(0.898f, 0.850f, 0.858f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TextDisabled,     ImVec4(0.498f, 0.450f, 0.458f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_WindowBg,         ImVec4(0.1f  , 0.1f  , 0.1f  , 1.0f));
    ImGui::PushStyleColor(ImGuiCol_MenuBarBg,        ImVec4(0.5f  , 0.5f  , 0.5f  , 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Header,           ImVec4(0.2f  , 0.3f  , 0.6f  , 0.5f));
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered,    ImVec4(0.2f  , 0.3f  , 0.6f  , 1.0f));
    ImGui::PushStyleColor(ImGuiCol_HeaderActive,     ImVec4(0.2f  , 0.3f  , 0.6f  , 1.0f));
    ImGui::PushStyleColor(ImGuiCol_TitleBg,          ImVec4(0.5f  , 0.5f  , 0.5f  , 0.5f));
    ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, ImVec4(0.5f  , 0.5f  , 0.5f  , 0.5f));
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive,    ImVec4(0.2f  , 0.3f  , 0.6f  , 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Button,           ImVec4(0.1f  , 0.1f  , 0.1f  , 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,    ImVec4(0.45f , 0.72f , 0.72f , 0.5f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,     ImVec4(0.84f , 0.78f , 0.78f , 0.5f));
    ImGui::PushStyleColor(ImGuiCol_SliderGrab,       ImVec4(0.45f , 0.72f , 0.72f , 0.5f));
    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(0.847f, 0.780f, 0.650f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_TooltipBg,        ImVec4(0.0f  , 0.0f  , 0.0f  , 0.0f));
}

void InventoryController::init()
{
    m_leftHandEquipedItem  = std::string();
    m_rightHandEquipedItem = std::string();
    m_twoHandEquipedItem   = std::string();

    g_LockPlayerForInput = m_displayInventory = m_draggingItem = false;
    m_display_ui_info = m_display_ui_stats = m_display_ui_misc = m_display_ui_container = false;
    m_draggedItemPrevSlot = m_currentActivatedItemSlot = ITEM_NULL_ID;

    _inventory_data.size = irr::core::vector2di(g_PlayerData.inventoryData.size.X, g_PlayerData.inventoryData.size.Y);

    for (auto y = 0; y < _inventory_data.size.Y; y++) {
        for (auto x = 0; x < _inventory_data.size.X; x++) {
            _inventory_data.contents.emplace_back(Item());
        }
    }

    m_slotImage   = RenderManager::Get()->driver()->getTexture("content/texture/ui/gridslot.png");
    m_slotImage1H = RenderManager::Get()->driver()->getTexture("content/texture/ui/gridslot_1h.png");
    m_slotImage2H = RenderManager::Get()->driver()->getTexture("content/texture/ui/gridslot_2h.png");

    m_leftHandEquipSlot  = Item();
    m_rightHandEquipSlot = Item();
    m_twoHandEquipSlot   = Item();

    m_currentClickedItem = Item();
}

void InventoryController::update(PlayerData &data)
{
    static auto tab_pressed = false;
    if (InputManager::Get()->getKeyPressOnce(KEY_TAB, &tab_pressed, true)) 
	{
        m_displayInventory = !m_displayInventory;

        InputManager::Get()->centerMouse();
    }

	ImGui::GetIO().MouseDrawCursor = m_displayInventory;
	InputManager::Get()->canProcessInput(!m_displayInventory);

    g_PlayerInventoryIsDisplaying = m_displayInventory;

	static bool dragged_from_2h_slot = false, write_dropped_item_data = false;
	static entityid dropped_item_id;
	static std::string dropped_item_data;

	if (write_dropped_item_data)
	{
		auto ent = WorldManager::Get()->managerSystem()->getEntityByID(dropped_item_id);

		if (ent.hasComponent<ItemComponent>())
		{
			ent.getComponent<ItemComponent>().data = dropped_item_data;
		}

		write_dropped_item_data = false;
		dropped_item_id = _entity_null_value;
		dropped_item_data = std::string();
	}

    if (m_displayInventory) 
	{
        SetIMGUI_PlayerInventoryTheme();

        // Mouse position local to window
        const auto mouse_position = irr::core::vector2di(
            static_cast<int>(ImGui::GetIO().MousePos.x),
            static_cast<int>(ImGui::GetIO().MousePos.y));

        for (auto y = 0; y < _inventory_data.size.Y; y++) 
		{
            for (auto x = 0; x < _inventory_data.size.X; x++) 
			{
                if (!m_draggingItem) 
                {
                    // Activate items
                    if (InputManager::Get()->isMouseButtonPressed(MB_RIGHT, true)) 
					{
                        if (_mouse_over_slot) 
						{
                            if (_inventory_data.contents.at(_current_gridslot).id < ITEM_NULL_ID) 
							{
                                if (!_inventory_data.contents.at(_current_gridslot).script.empty()) 
								{
                                    m_currentActivatedItemSlot = _current_gridslot;
                                    m_currentActivatedItem = _inventory_data.contents.at(_current_gridslot);

                                    ScriptManager::Get()->execute(
                                        _inventory_data.contents.at(_current_gridslot).script_component,
                                        _inventory_data.contents.at(_current_gridslot).script_component.onUseEventFunc,
                                        _inventory_data.contents.at(_current_gridslot).id);
                                }
                            }
                        }
                    }

                    bool cursor_out_of_bounds = false;

                    // Detect item dragging
                    if (InputManager::Get()->isMouseButtonPressed(MB_LEFT, true)) 
					{
                        if (_mouse_over_slot) 
						{
                            if (_inventory_data.contents.at(_current_gridslot).id < ITEM_NULL_ID) 
							{
                                m_currentClickedItem = m_currentDraggedItem = _inventory_data.contents.at(_current_gridslot);
                                m_draggedItemPrevSlot = _current_gridslot;
                                m_draggingItem = true;
                            }
                        }

                        if (_mouse_over_equip_slot_2h) 
						{
                            if (m_twoHandEquipSlot.id < ITEM_NULL_ID) 
							{
                                m_currentDraggedItem = m_twoHandEquipSlot;
                                m_draggedItemPrevSlot = ITEM_NULL_ID;
                                m_draggingItem = true;

								dragged_from_2h_slot = true;
                            }
                        }
                    }
                }
            }
        }

        // If dragging item
        if (m_draggingItem) {
            if (!InputManager::Get()->isMouseButtonPressed(MB_LEFT, true)) 
			{
                m_draggingItem = false;

                bool is_mouse_out_of_bounds = true;

                for (auto y = 0; y < _inventory_data.size.Y; y++) 
				{
                    for (auto x = 0; x < _inventory_data.size.X; x++) 
					{
                        if (_mouse_over_slot) 
						{
                            is_mouse_out_of_bounds = false;

                            if (_inventory_data.contents.at(_current_gridslot).id == ITEM_NULL_ID) 
							{
                                _inventory_data.contents.at(_current_gridslot) = m_currentDraggedItem;

                                if (m_draggedItemPrevSlot < ITEM_NULL_ID) 
								{
                                    _inventory_data.contents.at(m_draggedItemPrevSlot) = Item();
                                }

                                m_draggedItemPrevSlot = ITEM_NULL_ID;
                                m_currentDraggedItem = Item();

								if (dragged_from_2h_slot)
								{
									m_twoHandEquipSlot = Item();
									dragged_from_2h_slot = false;
								}

                                break;
                            }
                        }

                        // Equip item
                        if (_mouse_over_equip_slot_2h) 
						{
                            is_mouse_out_of_bounds = false;

                            // Both 1H and 2H for now
                            if ((m_currentDraggedItem.equip1H || m_currentDraggedItem.equip2H) && m_twoHandEquipSlot.id == ITEM_NULL_ID) 
							{
                                this->equipTwoHand(m_currentDraggedItem);

                                data.isWeaponEquipped = true;

                                if (m_draggedItemPrevSlot < ITEM_NULL_ID) 
								{
                                    _inventory_data.contents.at(m_draggedItemPrevSlot) = Item();
                                }

                                m_draggedItemPrevSlot = ITEM_NULL_ID;
                                m_currentDraggedItem = Item();

                                break;
                            }

                            // If something is already equipped
                            if ((m_currentDraggedItem.equip1H || m_currentDraggedItem.equip2H) && m_twoHandEquipSlot.id
	                            != ITEM_NULL_ID && !dragged_from_2h_slot)
                            {
	                            auto unequippedItem = this->getItemTwoHand();
	                            this->unequipTwoHand();

	                            this->equipTwoHand(m_currentDraggedItem);

	                            data.isWeaponEquipped = true;

	                            if (m_draggedItemPrevSlot < ITEM_NULL_ID)
	                            {
		                            _inventory_data.contents.at(m_draggedItemPrevSlot) = Item();
	                            }

	                            m_draggedItemPrevSlot = ITEM_NULL_ID;
	                            m_currentDraggedItem = Item();

	                            pickupItem(unequippedItem);

	                            break;
                            }
                        }
                    }
                }

                // Drop item
                if (is_mouse_out_of_bounds) 
				{
                    if (!m_currentDraggedItem.entity.empty()) 
					{
                        auto &player = WorldManager::Get()->managerSystem()->getEntityByName("player");
                        if (player.isValid()) 
						{
                            auto hit = RenderManager::Get()->raycastWorldPosition(
                                player.getComponent<CameraComponent>().camera->getAbsolutePosition(),
                                player.getComponent<CameraComponent>().nearTargetNode->getAbsolutePosition(),
                                true);

                            if (!hit.hit) 
							{
                                auto id = WorldManager::Get()->spawnEntity(_asset_ent(m_currentDraggedItem.entity), "", false,
                                    player.getComponent<CameraComponent>().nearTargetNode->getAbsolutePosition());

								write_dropped_item_data = true;
								dropped_item_id = id;
								dropped_item_data = m_currentDraggedItem.data;

                                if (m_draggedItemPrevSlot < ITEM_NULL_ID) 
								{
                                    _inventory_data.contents.at(m_draggedItemPrevSlot) = Item();
                                }
                                else if (m_currentDraggedItem.id == m_twoHandEquipSlot.id) 
								{
                                    this->unequipTwoHand();
                                }

                                m_currentDraggedItem = Item();

                                m_draggedItemPrevSlot = ITEM_NULL_ID;
                            }

							dragged_from_2h_slot = false;
                        }
                    }
                }
            }
        }

        // Unequip item
        if (m_twoHandEquipSlot.id == ITEM_NULL_ID) 
		{
            this->unequipTwoHand();

            data.isWeaponEquipped = false;
        }

        draw(mouse_position);

        // UI
        SetNextWindowPos(ImVec2((RenderManager::Get()->getConfiguration().width / 2) - 104, RenderManager::Get()->getConfiguration().height - 48));
        PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
        Begin("player_ui_selector", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove); {

            static bool info_pressed = false;
            if (Button(" Info ", ImVec2(64, 32)) && !info_pressed) {
                m_display_ui_info = !m_display_ui_info;

                info_pressed = true;

                m_display_ui_stats = false;
                m_display_ui_misc = false;
            } else {
                info_pressed = false;
            }

            SameLine();

            static bool skills_pressed = false;
            if (Button(" Stat ", ImVec2(64, 32)) && !skills_pressed) {
                m_display_ui_stats = !m_display_ui_stats;

                skills_pressed = true;

                m_display_ui_info = false;
                m_display_ui_misc = false;
            }
            else {
                skills_pressed = false;
            }

            SameLine();

            static bool misc_pressed = false;
            if (Button(" Misc ", ImVec2(64, 32)) && !misc_pressed) {
                m_display_ui_misc = !m_display_ui_misc;

                misc_pressed = true;

                m_display_ui_stats = false;
                m_display_ui_info = false;
            }
            else {
                misc_pressed = false;
            }
        }
        End();
        PopStyleColor(2);

        if (m_display_ui_info) {
            SetNextWindowPos(ImVec2(0, 0));
            SetNextWindowSize(ImVec2(280, 400));
            Begin("player_ui_info", nullptr, ImGuiWindowFlags_AlwaysAutoResize |
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove); {
                if (m_currentClickedItem.name != "null_item") {
                    Text(m_currentClickedItem.name.c_str());
                    Spacing();
                    Text(m_currentClickedItem.desc.c_str());
                }
            }
            End();
        }

        if (m_display_ui_stats) {
            SetNextWindowPos(ImVec2(0, 0));
            SetNextWindowSize(ImVec2(280, 400));
            Begin("player_ui_stats", nullptr, ImGuiWindowFlags_AlwaysAutoResize |
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove); {

                Text("Health: %i", data.currentHealth);

                Spacing();
                Spacing();
                Spacing();

                static int points = 5, str = 3, end = 5, agl = 3, itl = 6, per = 4;
                std::string display_bar_str;
                Text("- Skills -");
                Spacing();
                Text("Available Points: %i", points);
                Spacing();
                Text("Strength:     %i", str); SameLine(); for (auto i = 0; i < 10; i++) { if (i - str > 0) { display_bar_str += " "; } else { display_bar_str += "="; } } Text(display_bar_str.c_str()); SameLine(); PushID("increment_skill_str"); if (Button("+", ImVec2(16, 16)) && points) { str++; points--; } PopID(); display_bar_str = "";
                Text("Endurance:    %i", end); SameLine(); for (auto i = 0; i < 10; i++) { if (i - end > 0) { display_bar_str += " "; } else { display_bar_str += "="; } } Text(display_bar_str.c_str()); SameLine(); PushID("increment_skill_end"); if (Button("+", ImVec2(16, 16)) && points) { end++; points--; } PopID(); display_bar_str = "";
                Text("Agility:      %i", agl); SameLine(); for (auto i = 0; i < 10; i++) { if (i - agl > 0) { display_bar_str += " "; } else { display_bar_str += "="; } } Text(display_bar_str.c_str()); SameLine(); PushID("increment_skill_agl"); if (Button("+", ImVec2(16, 16)) && points) { agl++; points--; } PopID(); display_bar_str = "";
                Text("Intelligence: %i", itl); SameLine(); for (auto i = 0; i < 10; i++) { if (i - itl > 0) { display_bar_str += " "; } else { display_bar_str += "="; } } Text(display_bar_str.c_str()); SameLine(); PushID("increment_skill_itl"); if (Button("+", ImVec2(16, 16)) && points) { itl++; points--; } PopID(); display_bar_str = "";
                Text("Perception:   %i", per); SameLine(); for (auto i = 0; i < 10; i++) { if (i - per > 0) { display_bar_str += " "; } else { display_bar_str += "="; } } Text(display_bar_str.c_str()); SameLine(); PushID("increment_skill_per"); if (Button("+", ImVec2(16, 16)) && points) { per++; points--; } PopID(); display_bar_str = "";
            }
            End();
        }

        if (m_display_ui_misc) {
            SetNextWindowPos(ImVec2(0, 0));
            SetNextWindowSize(ImVec2(280, 400));
            Begin("player_ui_misc", nullptr, ImGuiWindowFlags_AlwaysAutoResize |
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove); {
                static bool pause;
                static ImVector<float> values; if (values.empty()) { values.resize(90); memset(values.Data, 0, values.Size * sizeof(float)); }
                static int values_offset = 0;
                if (!pause)
                {
                    static float refresh_time = GetTime(); // Create dummy data at fixed 60 hz rate for the demo
                    for (; GetTime() > refresh_time + 1.0f / 60.0f; refresh_time += 1.0f / 60.0f)
                    {
                        static float phase = 0.0f;
                        values[values_offset] = cosf(phase);
                        values_offset = (values_offset + 1) % values.Size;
                        phase += 0.10f*values_offset;
                    }
                }
                PlotLines("##Lines", values.Data, values.Size, values_offset, "avg 0.0", -1.0f, 1.0f, ImVec2(0, 80));
                SameLine(0, GetStyle().ItemInnerSpacing.x);
                BeginGroup();
                Text("Lines");
                Checkbox("pause", &pause);
                EndGroup();
            }
            End();
        }

        if (m_display_ui_container) {
            SetNextWindowPos(ImVec2(RenderManager::Get()->getConfiguration().width - 280, 0));
            SetNextWindowSize(ImVec2(280, 400));
            Begin("player_ui_container", nullptr, ImGuiWindowFlags_AlwaysAutoResize |
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove); {
            }
            End();
        }

        PopStyleColor(16);
    }
}

void InventoryController::draw(irr::core::vector2di mouse_position)
{
    for (auto y = 0; y < _inventory_data.size.Y; y++) {
        for (auto x = 0; x < _inventory_data.size.X; x++) {

            // Grid slots
            RenderManager::Get()->renderImage2D(m_slotImage, _slot_position_vec2);

            if (_inventory_data.contents.at(_current_gridslot).id < ITEM_NULL_ID) {
                // Item icons
                RenderManager::Get()->renderImage2D(_inventory_data.contents.at(_current_gridslot).iconTexture, _slot_position_vec2);
            
                if (_mouse_over_slot && !m_draggingItem) {
                    // Tooltips
                    auto tooltip = irr::core::stringw(_inventory_data.contents.at(_current_gridslot).name.c_str());
                    RenderManager::Get()->renderText2D(
                        tooltip,
                        TEXT_DEFAULT_FONT::SMALL,
                        irr::core::rect<irr::s32>(mouse_position.X, mouse_position.Y + ITEM_NAME_TOOLTIP_OFFSET_Y, 0, 0),
                        irr::video::SColor(255, 255, 255, 255));
                }
            }
        }
    }

    // Equip slot
    {
        RenderManager::Get()->renderImage2D(m_slotImage2H, _equip_slot_position_2h);

        if (m_twoHandEquipSlot.id < ITEM_NULL_ID) {
            RenderManager::Get()->renderImage2D(m_twoHandEquipSlot.iconEquipTexture, _equip_slot_position_2h);

            if (m_twoHandEquipSlot.data != "-1") {
                auto ammo_text = irr::core::stringw(std::to_string(getCurrentWeaponAmmoCount()).c_str());
                RenderManager::Get()->renderText2D(
                    ammo_text,
                    TEXT_DEFAULT_FONT::SMALL,
                    irr::core::rect<irr::s32>(_equip_slot_position_2h.X, _equip_slot_position_2h.Y, 0, 0),
                    irr::video::SColor(255, 255, 255, 255));
            }
        }
    }

    if (m_draggingItem) {
        RenderManager::Get()->renderImage2D(m_currentDraggedItem.iconTexture, mouse_position + irr::core::vector2di(-32, -32));
    }
}

void InventoryController::destroy()
{
    m_slotImage   = nullptr;
    m_slotImage2H = nullptr;
    m_slotImage1H = nullptr;
}

bool InventoryController::pickupItem(itemid item, entityid ent)
{
    for (auto i = 0U; i < _inventory_data.contents.size(); i++) {
        if (_inventory_data.contents.at(i).id == ITEM_NULL_ID) {
			_inventory_data.contents.at(i) = ItemDatabase::GetItemByID(item);

            if (ent < ITEM_NULL_ID) {
                if (WorldManager::Get()->managerSystem()->getEntityByID(ent).hasComponent<ItemComponent>()) {
                    auto& item_component = WorldManager::Get()->managerSystem()->getEntityByID(ent).getComponent<ItemComponent>();

                    if (!item_component.data.empty()) {
                        _inventory_data.contents.at(i).data = item_component.data;
                    }
                }
            }

            return true;
        }
    }

    return false;
}
bool InventoryController::pickupItem(std::string item, entityid ent)
{
    for (auto i = 0U; i < _inventory_data.contents.size(); i++) {
        if (_inventory_data.contents.at(i).id == ITEM_NULL_ID) {
            _inventory_data.contents.at(i) = ItemDatabase::GetItemByName(item);

            if (ent < ITEM_NULL_ID) {
                if (WorldManager::Get()->managerSystem()->getEntityByID(ent).hasComponent<ItemComponent>()) {
                    auto& item_component = WorldManager::Get()->managerSystem()->getEntityByID(ent).getComponent<ItemComponent>();

                    if (!item_component.data.empty()) {
                        _inventory_data.contents.at(i).data = item_component.data;
                    }
                }
            }

            return true;
        }
    }

    return false;
}

bool InventoryController::pickupItem(const Item& item)
{
    for (auto i = 0U; i < _inventory_data.contents.size(); i++) {
        if (_inventory_data.contents.at(i).id == ITEM_NULL_ID) {
            _inventory_data.contents.at(i) = item;

            return true;
        }
    }

    return false;
}

bool InventoryController::dropItem(unsigned int slot)
{
   if (slot > _inventory_data.contents.size()) {
       return false;
   }

    auto &player = WorldManager::Get()->managerSystem()->getEntityByName("player");
    if (!player.isValid()) {
        return false;
    }

   auto hit = RenderManager::Get()->raycastWorldPosition(
       player.getComponent<CameraComponent>().camera->getAbsolutePosition(),
       player.getComponent<CameraComponent>().nearTargetNode->getAbsolutePosition(),
       true);

   if (!hit.hit) {
       WorldManager::Get()->spawnEntity(_inventory_data.contents.at(slot).entity, "", false,
           player.getComponent<CameraComponent>().nearTargetNode->getAbsolutePosition());
       _inventory_data.contents.at(slot) = Item();

       return true;
   }

   return false;
}

bool InventoryController::removeItem(unsigned int slot)
{
    if (slot > _inventory_data.contents.size()) {
        return false;
    }

    _inventory_data.contents.at(slot) = Item();

    return true;
}

int InventoryController::getFirstItemSlotOfType(itemid id)
{
    for (auto i = 0U; i < _inventory_data.contents.size(); i++) {
        if (_inventory_data.contents.at(i).id == id) {
            return i;
        }
    }

    return -1;
}
int InventoryController::getFirstItemSlotOfType(std::string name)
{
    for (auto i = 0U; i < _inventory_data.contents.size(); i++) {
        if (_inventory_data.contents.at(i).name == name) {
            return i;
        }
    }

    return -1;
}

// HACK for warning C4172
static Item g_item;
Item& InventoryController::getFirstItemCopyOfType(itemid id)
{
    for (auto i = 0U; i < _inventory_data.contents.size(); i++) {
        if (_inventory_data.contents.at(i).id == id) {
            return _inventory_data.contents.at(i);
        }
    }

    return g_item;
}
Item& InventoryController::getFirstItemCopyOfType(std::string name)
{
    for (auto i = 0U; i < _inventory_data.contents.size(); i++) {
        if (_inventory_data.contents.at(i).name == name) {
            return _inventory_data.contents.at(i);
        }
    }

    return g_item;
}

bool InventoryController::equipLeftHand(const Item& item)
{
    m_leftHandEquipSlot = item;

    m_leftHandEquipedItem = item.refEntity;

    WorldManager::Get()->spawnEntity(_asset_ent(item.refEntity), item.refEntity);

    return true;
}
bool InventoryController::equipRightHand(const Item& item)
{
    m_rightHandEquipSlot = item;

    m_rightHandEquipedItem = item.refEntity;

    WorldManager::Get()->spawnEntity(_asset_ent(item.refEntity), item.refEntity);

    return true;
}
bool InventoryController::equipTwoHand(const Item& item)
{
    m_twoHandEquipSlot = item;

    m_twoHandEquipedItem = item.refEntity;

    WorldManager::Get()->spawnEntity(_asset_ent(item.refEntity), item.refEntity);

    return true;
}

bool InventoryController::unequipLeftHand()
{
    m_leftHandEquipSlot = Item();

    if (!m_leftHandEquipedItem.empty()) {
        WorldManager::Get()->killEntityByName(m_twoHandEquipedItem);
        m_leftHandEquipedItem = std::string();
    }

    return true;
}
bool InventoryController::unequipRightHand()
{
    m_rightHandEquipSlot = Item();

    if (!m_rightHandEquipedItem.empty()) {
        WorldManager::Get()->killEntityByName(m_twoHandEquipedItem);
        m_rightHandEquipedItem = std::string();
    }

    return true;
}
bool InventoryController::unequipTwoHand()
{
    m_twoHandEquipSlot = Item();

    if (!m_twoHandEquipedItem.empty()) {
        WorldManager::Get()->killEntityByName(m_twoHandEquipedItem);
        m_twoHandEquipedItem = std::string();
    }

    return true;
}

int InventoryController::getCurrentWeaponAmmoCount()
{
    if (isEquipedTwoHand()) {
        auto value = getItemTwoHand().data;

        if (getItemTwoHand().data == "-1") {
            return -1;
        }

        return atoi(value.c_str());
    }

    return -1;
}

void InventoryController::removeCurrentActivatedItem()
{
    g_PlayerData.inventoryData.contents.at(m_currentActivatedItemSlot) = Item();
}
