#ifndef SCENE_H
#define SCENE_H

#include <entt/entity/entity.hpp>
#include <entt/entity/registry.hpp>
#include <entt/entity/component.hpp>

class Scene
{
	friend class Entity;
public:
	Scene() {}

	template<typename T>
	inline entt::view<T> GetEntities()						{ return m_Registry.view<T>(); }

	inline entt::storage<entt::entity>& GetAllEntities()	{ return m_Registry.storage<entt::entity>(); }
	inline entt::entity CreateEntity()						{ return m_Registry.create(); }
private:
	entt::registry m_Registry;
};

#endif