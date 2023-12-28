#pragma once

#include <IMGUI/imgui.h>
#include <IMGUI/imgui_internal.h>

#if 0
void EditTransform(const Camera& camera, matrix_t& matrix)
{
	static ImTransformControl::OPERATION mCurrentGizmoOperation(ImTransformControl::ROTATE);
	static ImTransformControl::MODE mCurrentGizmoMode(ImTransformControl::WORLD);
	if (ImGui::IsKeyPressed(90))
		mCurrentGizmoOperation = ImTransformControl::TRANSLATE;
	if (ImGui::IsKeyPressed(69))
		mCurrentGizmoOperation = ImTransformControl::ROTATE;
	if (ImGui::IsKeyPressed(82)) // r Key
		mCurrentGizmoOperation = ImTransformControl::SCALE;
	if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImTransformControl::TRANSLATE))
		mCurrentGizmoOperation = ImTransformControl::TRANSLATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImTransformControl::ROTATE))
		mCurrentGizmoOperation = ImTransformControl::ROTATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImTransformControl::SCALE))
		mCurrentGizmoOperation = ImTransformControl::SCALE;
	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	ImTransformControl::DecomposeMatrixToComponents(matrix.m16, matrixTranslation, matrixRotation, matrixScale);
	ImGui::InputFloat3("Tr", matrixTranslation, 3);
	ImGui::InputFloat3("Rt", matrixRotation, 3);
	ImGui::InputFloat3("Sc", matrixScale, 3);
	ImTransformControl::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix.m16);

	if (mCurrentGizmoOperation != ImTransformControl::SCALE)
	{
		if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImTransformControl::LOCAL))
			mCurrentGizmoMode = ImTransformControl::LOCAL;
		ImGui::SameLine();
		if (ImGui::RadioButton("World", mCurrentGizmoMode == ImTransformControl::WORLD))
			mCurrentGizmoMode = ImTransformControl::WORLD;
	}
	static bool useSnap(false);
	if (ImGui::IsKeyPressed(83))
		useSnap = !useSnap;
	ImGui::Checkbox("", &useSnap);
	ImGui::SameLine();
	vec_t snap;
	switch (mCurrentGizmoOperation)
	{
	case ImTransformControl::TRANSLATE:
		snap = config.mSnapTranslation;
		ImGui::InputFloat3("Snap", &snap.x);
		break;
	case ImTransformControl::ROTATE:
		snap = config.mSnapRotation;
		ImGui::InputFloat("Angle Snap", &snap.x);
		break;
	case ImTransformControl::SCALE:
		snap = config.mSnapScale;
		ImGui::InputFloat("Scale Snap", &snap.x);
		break;
	}
	ImGuiIO& io = ImGui::GetIO();
	ImTransformControl::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImTransformControl::Manipulate(camera.mView.m16, camera.mProjection.m16, mCurrentGizmoOperation, mCurrentGizmoMode, matrix.m16, NULL, useSnap ? &snap.x : NULL);
}
#endif
#pragma once

#ifdef USE_IMGUI_API
#include "imconfig.h"
#endif
#ifndef IMGUI_API
#define IMGUI_API
#endif

namespace ImTransformControl
{
	// call inside your own window and before Manipulate() in order to draw gizmo to that window.
	IMGUI_API void SetDrawlist();

	// call BeginFrame right after ImGui_XXXX_NewFrame();
	IMGUI_API void BeginFrame();

	// return true if mouse cursor is over any gizmo control (axis, plan or screen component)
	IMGUI_API bool IsOver();

	// return true if mouse IsOver or if the gizmo is in moving state
	IMGUI_API bool IsUsing();

	// enable/disable the gizmo. Stay in the state until next call to Enable.
	// gizmo is rendered with gray half transparent color when disabled
	IMGUI_API void Enable(bool enable);

	// helper functions for manualy editing translation/rotation/scale with an input float
	// translation, rotation and scale float points to 3 floats each
	// Angles are in degrees (more suitable for human editing)
	// example:
	// float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	// ImTransformControl::DecomposeMatrixToComponents(gizmoMatrix.m16, matrixTranslation, matrixRotation, matrixScale);
	// ImGui::InputFloat3("Tr", matrixTranslation, 3);
	// ImGui::InputFloat3("Rt", matrixRotation, 3);
	// ImGui::InputFloat3("Sc", matrixScale, 3);
	// ImTransformControl::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, gizmoMatrix.m16);
	//
	// These functions have some numerical stability issues for now. Use with caution.
	IMGUI_API void DecomposeMatrixToComponents(const float *matrix, float *translation, float *rotation, float *scale);
	IMGUI_API void RecomposeMatrixFromComponents(const float *translation, const float *rotation, const float *scale, float *matrix);

	IMGUI_API void SetRect(float x, float y, float width, float height);
	// default is false
	IMGUI_API void SetOrthographic(bool isOrthographic);

	// Render a cube with face color corresponding to face normal. Usefull for debug/tests
	IMGUI_API void DrawCube(const float *view, const float *projection, const float *matrix);
	IMGUI_API void DrawGrid(const float *view, const float *projection, const float *matrix, const float gridSize);

	// call it when you want a gizmo
	// Needs view and projection matrices. 
	// matrix parameter is the source matrix (where will be gizmo be drawn) and might be transformed by the function. Return deltaMatrix is optional
	// translation is applied in world space
	enum OPERATION
	{
		TRANSLATE,
		ROTATE,
		SCALE,
		BOUNDS,
	};

	enum MODE
	{
		LOCAL,
		WORLD
	};

	IMGUI_API void Manipulate(const float *view, const float *projection, OPERATION operation, MODE mode, float *matrix, float *deltaMatrix = 0, float *snap = 0, float *localBounds = NULL, float *boundsSnap = NULL);
};
