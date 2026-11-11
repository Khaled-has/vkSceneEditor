#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

class Camera
{
public:
	Camera(float mSpeed = 25.0f)
		: m_Speed(mSpeed)
	{}

	const glm::mat4& SetupProjection(float mNearPlane, float mFarPlane, glm::vec2 mAspectRatio);
	const glm::mat4& UpdateView(glm::vec3 mPosOffset, glm::vec2 mLookOffset, glm::uvec4 mMove);

	inline const glm::mat4& GetProjMat()		const { return m_ProjMat; }
	inline const glm::mat4& GetViewMat()		const { return m_ViewMat; }

	inline const glm::vec3& GetCamPos()			const { return m_CamPos; }
	inline const glm::vec2& GetAspectRatio()	const { return m_AspectRatio; }

	inline float GetNearPlane()					const { return m_NearPlane; }
	inline float GetFarPlane()					const { return m_FarPlane; }

private:
	float m_Speed;
	float m_Yaw = 0.0f;
	float m_Pitch = -90.0f;
	float m_Sensitivity = 0.15f;

	glm::vec3 m_WorldUp = glm::vec3(0, 1.0f, 0);

	glm::vec3 m_Front;
	glm::vec3 m_Right;
	glm::vec3 m_Up;

	glm::vec3 m_CamPos;
	glm::vec2 m_AspectRatio;

	float m_NearPlane;
	float m_FarPlane;

	glm::mat4 m_ProjMat;
	glm::mat4 m_ViewMat;
};

#endif