#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <iostream>
#include <glm/glm.hpp>

struct TagComponent
{
	std::string mTag;
};

struct TransformComponent
{
	glm::vec3 mPosition;
	glm::vec3 mScale;
	glm::vec3 mRotation;
};

struct MeshComponent
{
	std::string mModelName;
	uint32_t mMeshID;
};

#endif