#include "ImGuiExtensions.h"

#include <array>

#include <IMGUI/imgui.h>
#include <ImGUI/imgui_internal.h>

#include "Engine/Engine.h"

using namespace ImGui;

bool ImColorPicker(const char* label, ImColor* color)
{
	static const float HUE_PICKER_WIDTH = 65.0f;
	static const ImVec2 SV_PICKER_SIZE = ImVec2(0, 200);

	bool value_changed = false;

	ImDrawList* draw_list = GetWindowDrawList();

	ImVec2 picker_pos = GetCursorScreenPos();

	float hue, saturation, value;
	ColorConvertRGBtoHSV(
		color->Value.x, color->Value.y, color->Value.z, hue, saturation, value);
	auto hue_color = ImColor::HSV(hue, 1, 1);

	float rr, gg, bb;
    ColorConvertHSVtoRGB(hue, 1, 1, rr, gg, bb);
    
    // Hue
	ImColor colors[] = {
		ImColor(255, 0, 0),
		ImColor(255, 255, 0),
		ImColor(0, 255, 0),
		ImColor(0, 255, 255),
		ImColor(0, 0, 255),
		ImColor(255, 0, 255),
		ImColor(255, 0, 0) };
    
	for (int i = 0; i < 6; ++i) {
		draw_list->AddRectFilledMultiColor(
			ImVec2(picker_pos.x + SV_PICKER_SIZE.x, picker_pos.y + i * (SV_PICKER_SIZE.y / 6)),
			ImVec2(picker_pos.x + SV_PICKER_SIZE.x + HUE_PICKER_WIDTH,
				picker_pos.y + (i + 1) * (SV_PICKER_SIZE.y / 6)),
			colors[i],
			colors[i],
			colors[i + 1],
			colors[i + 1]);
	}
	draw_list->AddLine(
		ImVec2(picker_pos.x + SV_PICKER_SIZE.x, picker_pos.y + hue * SV_PICKER_SIZE.y),
		ImVec2(picker_pos.x + SV_PICKER_SIZE.x + HUE_PICKER_WIDTH,
			picker_pos.y + hue * SV_PICKER_SIZE.y),
		ImColor(255, 255, 255), 2.0f);
    
	InvisibleButton("hue_selector", ImVec2(HUE_PICKER_WIDTH, SV_PICKER_SIZE.y));
	if (IsItemHovered()) {
		if (GetIO().MouseDown[0]) {
			hue = ((GetIO().MousePos.y - picker_pos.y) / SV_PICKER_SIZE.y);
			value_changed = true;
		}
	}

	// Saturation
	draw_list->AddRectFilledMultiColor(
		ImVec2(picker_pos.x + SV_PICKER_SIZE.x + HUE_PICKER_WIDTH, picker_pos.y),
		ImVec2(picker_pos.x + SV_PICKER_SIZE.x + HUE_PICKER_WIDTH * 2, picker_pos.y + SV_PICKER_SIZE.y),
		ImColor(255, 255, 255),
		ImColor(255, 255, 255),
		ImColor(rr, gg, bb),
		ImColor(rr, gg, bb));

	draw_list->AddLine(
		ImVec2(picker_pos.x + SV_PICKER_SIZE.x + HUE_PICKER_WIDTH, picker_pos.y + saturation * SV_PICKER_SIZE.y),
		ImVec2(picker_pos.x + SV_PICKER_SIZE.x + HUE_PICKER_WIDTH * 2,
			picker_pos.y + saturation * SV_PICKER_SIZE.y),
		ImColor(255, 255, 255), 2.0f);

	SameLine();
	InvisibleButton("sat_selector", ImVec2(HUE_PICKER_WIDTH, SV_PICKER_SIZE.y));
	if (IsItemHovered()) {
		if (GetIO().MouseDown[0]) {
			saturation = ((GetIO().MousePos.y - picker_pos.y) / SV_PICKER_SIZE.y);
			value_changed = true;
		}
	}

	// Value
	draw_list->AddRectFilledMultiColor(
		ImVec2(picker_pos.x + SV_PICKER_SIZE.x + HUE_PICKER_WIDTH * 2, picker_pos.y),
		ImVec2(picker_pos.x + SV_PICKER_SIZE.x + HUE_PICKER_WIDTH * 3, picker_pos.y + SV_PICKER_SIZE.y),
		ImColor(0, 0, 0),
		ImColor(0, 0, 0),
		ImColor(255, 255, 255),
		ImColor(255, 255, 255));

	draw_list->AddLine(
		ImVec2(picker_pos.x + SV_PICKER_SIZE.x + HUE_PICKER_WIDTH * 2, picker_pos.y + value * SV_PICKER_SIZE.y),
		ImVec2(picker_pos.x + SV_PICKER_SIZE.x + HUE_PICKER_WIDTH * 3,
			picker_pos.y + value * SV_PICKER_SIZE.y),
		ImColor(255, 255, 255), 2.0f);

	SameLine();
	InvisibleButton("val_selector", ImVec2(HUE_PICKER_WIDTH, SV_PICKER_SIZE.y));
	if (IsItemHovered()) {
		if (GetIO().MouseDown[0]) {
			value = ((GetIO().MousePos.y - picker_pos.y) / SV_PICKER_SIZE.y);
			value_changed = true;
		}
	}

    
	*color = ImColor::HSV(hue, saturation, value);
	return value_changed | ColorEdit3(label, &color->Value.x);
}

/*float x = saturation * value;
ImVec2 p(picker_pos.x + x * SV_PICKER_SIZE.x, picker_pos.y + value * SV_PICKER_SIZE.y);
draw_list->AddLine(ImVec2(p.x - CROSSHAIR_SIZE, p.y), ImVec2(p.x - 2, p.y), ImColor(255, 255, 255));
draw_list->AddLine(ImVec2(p.x + CROSSHAIR_SIZE, p.y), ImVec2(p.x + 2, p.y), ImColor(255, 255, 255));
draw_list->AddLine(ImVec2(p.x, p.y + CROSSHAIR_SIZE), ImVec2(p.x, p.y + 2), ImColor(255, 255, 255));
draw_list->AddLine(ImVec2(p.x, p.y - CROSSHAIR_SIZE), ImVec2(p.x, p.y - 2), ImColor(255, 255, 255));

InvisibleButton("saturation_value_selector", SV_PICKER_SIZE);
if (IsItemHovered())
{
ImVec2 mouse_pos_in_canvas = ImVec2(
GetIO().MousePos.x - picker_pos.x, GetIO().MousePos.y - picker_pos.y);
if (GetIO().MouseDown[0])
{
mouse_pos_in_canvas.x =
ImMin(mouse_pos_in_canvas.x, mouse_pos_in_canvas.y);

value = mouse_pos_in_canvas.y / SV_PICKER_SIZE.y;
saturation = value == 0 ? 0 : (mouse_pos_in_canvas.x / SV_PICKER_SIZE.x) / value;
value_changed = true;
}
}

SetCursorScreenPos(ImVec2(picker_pos.x + SV_PICKER_SIZE.x + 10, picker_pos.y));

SetCursorScreenPos(ImVec2(picker_pos.x + SV_PICKER_SIZE.x + 10, picker_pos.y));*/