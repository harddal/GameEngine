#pragma once

#include <vector3d.h>
#include <ICameraSceneNode.h>

class EditorCamera
{
public:
    EditorCamera();
    
    void init();
    void update();
    void destroy();

    void reset();

    irr::core::vector3df getLookAt() const;
    irr::core::vector3df getLookAtNormalized() const;

private:
	irr::core::vector3df
		m_offset,
		m_target,
		m_lookat;

	irr::scene::ICameraSceneNode* m_camera;
	irr::scene::ISceneNode* m_targetNode;
};
