#include "FreeCameraController.h"

#include "PlayerData.h"
#include "Engine/Input/InputManager.h"
#include "Engine/Renderer/RenderManager.h"
#include "Engine/World/WorldManager.h"

std::unique_ptr<FreeCameraController> g_FreeCameraController;

FreeCameraController::FreeCameraController()
{

}

void FreeCameraController::init()
{

}

void FreeCameraController::update(float dt)
{
	anax::Entity& player = WorldManager::Get()->managerSystem()->getEntityByName("freecamera");

	if (!player.isValid()) {
		return;
	}

	auto& transform = player.getComponent<TransformComponent>();
	auto& camera = player.getComponent<CameraComponent>();

	if (m_firstUpdate)
	{
		transform.setRotation(irr::core::vector3df(0.0f, 0.0f, 0.0f));
		m_firstUpdate = false;
	}

	const float
		pi = 3.141592741f,
		pi_180 = 0.017453293f,
		sensitivity = 0.25f,
		maxXAngle = 82.0f,
		minXAngle = -82.0f,
		moveSpeed = 0.15f,
		strafeOffset = 1.570796370f;

	auto cameraRotation = camera.camera->getRotation();

	auto mouseDelta = InputManager::Get()->getMouseDelta();

	cameraRotation.Y -= mouseDelta.X * sensitivity;
	cameraRotation.X -= mouseDelta.Y * sensitivity;

	if (cameraRotation.X > maxXAngle)
		cameraRotation.X = maxXAngle;
	else if (cameraRotation.X < minXAngle)
		cameraRotation.X = minXAngle;

	camera.camera->setRotation(cameraRotation);
	transform.setRotation(irr::core::vector3df(transform.getRotation().X, camera.camera->getRotation().Y, 0.0f));

	float
		move = 0.0f,
		strafe = 0.0f;

	if (InputManager::Get()->isActionPressed("forward"))
	{
		if (InputManager::Get()->isActionPressed("sprint"))
		{
			move += moveSpeed * 2;
		}
		else
		{
			move += moveSpeed;
		}
	}
	if (InputManager::Get()->isActionPressed("backward"))
		move -= moveSpeed;
	if (InputManager::Get()->isActionPressed("strafel"))
		strafe -= moveSpeed;
	if (InputManager::Get()->isActionPressed("strafer"))
		strafe += moveSpeed;

	if (InputManager::Get()->isActionPressed("jump"))
		transform.setPosition(transform.getPosition() + irr::core::vector3df(0.0f, 0.25f, 0.0f));

	if (InputManager::Get()->isActionPressed("crouch"))
		transform.setPosition(transform.getPosition() + irr::core::vector3df(0.0f, -0.25f, 0.0f));

	auto moveDirection = cameraRotation.Y * pi_180;

	transform.setPosition(transform.getPosition() + irr::core::vector3df(
		move * sin(moveDirection) + strafe * sin(moveDirection + strafeOffset),
		move * -sin(cameraRotation.X * pi_180),
		move * cos(moveDirection) + strafe * cos(moveDirection + strafeOffset)));
}

void FreeCameraController::destroy()
{
	m_firstUpdate = true;
}

void FreeCameraController::pause()
{

}

void FreeCameraController::resume()
{
	
}