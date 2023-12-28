#include "Editor/EditorCamera.h"

#include "Engine/Engine.h"

EditorCamera::EditorCamera() : 
    m_offset(irr::core::vector3df(0.0f, 0.0f, 0.0f)),
    m_target(irr::core::vector3df(0.0f, 0.0f, 100.0f)),
    m_lookat(irr::core::vector3df(0.0f, 0.0f, 0.0f)),
    m_camera(nullptr), m_targetNode(nullptr) {}

void EditorCamera::init()
{
    m_camera = RenderManager::Get()->sceneManager()->addCameraSceneNode();
    RenderManager::Get()->sceneManager()->setActiveCamera(m_camera);

    m_camera->setNearValue(0.01f);
    m_camera->setFarValue(1000.0f);

    m_camera->setPosition(irr::core::vector3df(0.0f, 0.0f, 0.0f));
    m_camera->setRotation(irr::core::vector3df(0.0f, 0.0f, 0.0f));

    m_targetNode = RenderManager::Get()->sceneManager()->addEmptySceneNode();
    m_targetNode->setPosition(m_target);
    m_targetNode->setParent(m_camera);

    m_camera->setPosition(
        irr::core::vector3df(0.0f, 0.0f, 0.0f) + m_offset);
    m_camera->setRotation(irr::core::vector3df(0.0f, 0.0f, 0.0f));
}

void EditorCamera::update()
{
    const float
        pi = 3.141592741f,
        pi_180 = 0.017453293f,
        sensitivity = 0.25f,
        maxXAngle = 82.0f,
        minXAngle = -82.0f,
        strafeOffset = 1.570796370f;

	float moveSpeed = 0.15f;

    auto cameraRotation = m_camera->getRotation();

	static bool center = false, right_mouse_release = false;
	static auto old_cursor_pos = irr::core::vector2df(0.0f, 0.0f);
	
    if (InputManager::Get()->isMouseButtonPressed(1, true))
    {
    	if (!center)
    	{
			center = true;

			RenderManager::Get()->device()->getCursorControl()->setVisible(false);

			old_cursor_pos = InputManager::Get()->getMousePosition();
    		
			InputManager::Get()->centerMouse();
    	}
		else
		{
			auto mouseDelta = InputManager::Get()->getMouseDelta();

			cameraRotation.Y -= mouseDelta.X * sensitivity;
			cameraRotation.X -= mouseDelta.Y * sensitivity;

			if (cameraRotation.X > maxXAngle)
				cameraRotation.X = maxXAngle;
			else
				if (cameraRotation.X < minXAngle)
					cameraRotation.X = minXAngle;

			m_camera->setRotation(cameraRotation);
		}
    }
	else
	{
		center = false;
	}
	if (InputManager::Get()->getMouseRelease(1, &right_mouse_release, true))
	{
		InputManager::Get()->setMousePosition(old_cursor_pos);
		RenderManager::Get()->device()->getCursorControl()->setVisible(true);
	}

    float
        move = 0.0f,
        strafe = 0.0f;

	if (InputManager::Get()->isActionPressed("sprint"))
	{
		moveSpeed *= 2;
	}

	if (InputManager::Get()->isActionPressed("forward"))
		move += moveSpeed;
    if (InputManager::Get()->isActionPressed("backward"))
        move -= moveSpeed;
    if (InputManager::Get()->isActionPressed("strafel"))
        strafe -= moveSpeed;
    if (InputManager::Get()->isActionPressed("strafer"))
        strafe += moveSpeed;

    if (InputManager::Get()->isActionPressed("jump"))
        m_camera->setPosition(m_camera->getAbsolutePosition() + irr::core::vector3df(0.0f, 0.35f, 0.0f));

    if (InputManager::Get()->isActionPressed("crouch"))
        m_camera->setPosition(m_camera->getAbsolutePosition() + irr::core::vector3df(0.0f, -0.35f, 0.0f));

    auto moveDirection = cameraRotation.Y * pi_180;

    m_camera->setPosition(m_camera->getAbsolutePosition() + irr::core::vector3df(
        move * sin(moveDirection) + strafe * sin(moveDirection + strafeOffset),
        move * -sin(cameraRotation.X * pi_180),
        move * cos(moveDirection) + strafe * cos(moveDirection + strafeOffset)));

    m_camera->updateAbsolutePosition();
    m_targetNode->updateAbsolutePosition();

    m_camera->setTarget(m_targetNode->getAbsolutePosition());

    m_lookat = irr::core::vector3df(
        sin(deg2rad(cameraRotation.Y)) * cos(deg2rad(cameraRotation.X)),
        -sin(deg2rad(cameraRotation.X)),
        cos(deg2rad(cameraRotation.Y)) * cos(deg2rad(cameraRotation.X)));
}

void EditorCamera::destroy()
{
    RenderManager::Get()->sceneManager()->addToDeletionQueue(m_camera);
    RenderManager::Get()->sceneManager()->addToDeletionQueue(m_targetNode);
}

void EditorCamera::reset()
{
    RenderManager::Get()->sceneManager()->setActiveCamera(m_camera);
}

irr::core::vector3df EditorCamera::getLookAt() const
{
    return m_lookat;
}
irr::core::vector3df EditorCamera::getLookAtNormalized() const
{
    auto lan = m_lookat;
    return lan.normalize();
}