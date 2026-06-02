#pragma once
#include "MySystem.hpp"

class ProjectileSystem : public MySystem {
public:

void update(ee::ecs::World& _world, float _dt) override 
{

	for (auto& id : m_entities) {
		auto& p = _world.getComponent<ProjectileComponent>(id);

		p.timer -= _dt;

		if (p.timer > 0) continue;

		_world.destroyEntity(id);
	}
}

};