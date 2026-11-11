#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

const glm::mat4& Camera::SetupProjection(float mNearPlane, float mFarPlane, glm::vec2 mAspectRatio)
{
	m_NearPlane = mNearPlane;
	m_FarPlane = mFarPlane;
	m_AspectRatio = mAspectRatio;

	m_ProjMat = glm::perspective(glm::radians(45.0f), mAspectRatio.x / mAspectRatio.y, mNearPlane, mFarPlane);
	return m_ProjMat;
}

const glm::mat4& Camera::UpdateView(glm::vec3 mPosOffset, glm::vec2 mLookOffset, glm::uvec4 mMove)
{
	m_CamPos += mPosOffset;

	// # --
	m_Yaw += (mLookOffset.x * m_Sensitivity);
	m_Pitch += (mLookOffset.y * m_Sensitivity);

	if (m_Pitch > 89.f)	 m_Pitch = 89.f;
	if (m_Pitch < -89.f) m_Pitch = -89.f;

	glm::vec3 Front;
	Front.x = glm::cos(glm::radians(m_Yaw)) * glm::cos(glm::radians(m_Pitch));
	Front.y = glm::sin(glm::radians(m_Pitch));
	Front.z = glm::sin(glm::radians(m_Yaw)) * glm::cos(glm::radians(m_Pitch));
	m_Front = glm::normalize(Front);

	m_Right = glm::normalize(glm::cross(Front, m_Up));
	m_Up = glm::normalize(glm::cross(m_Right, Front));

	// # --
	float velocity = m_Speed * 0.016; // ms

	if (mMove.z)
	{
		m_CamPos += Front * velocity;
	}
	else if (mMove.w)
	{
		m_CamPos -= Front * velocity;
	}

	if (mMove.x)
	{
		m_CamPos += m_Right * velocity;
	}
	else if (mMove.y)
	{
		m_CamPos -= m_Right * velocity;
	}

	m_ViewMat = glm::lookAt(m_CamPos, m_CamPos + m_Front, m_Up);
	return m_ViewMat;
}
