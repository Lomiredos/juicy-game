#pragma once

#include "MySystem.hpp"

#include "../Components/TransformComponent.hpp"
#include "../Components/SphereComponent.hpp"
#include "../Components/MotionComponent.hpp"

#include "../EventsBus.hpp"
#include "../Events/PlayerEvent.hpp"
#include "../Events/SoundsEvent.hpp"
#include "rlgl.h"
#include "raymath.h"


class CollisionSystem : public MySystem {

	ee::ecs::EntityID m_playerID;
	bool              m_playerDead = false;

public:

	void init(ee::ecs::EntityID& _playerId)
	{
		m_playerID   = _playerId;
		m_playerDead = false;
	}

	void update(ee::ecs::World& _world, float _dt) override
	{
		if (m_playerDead) return;

		auto& pt = _world.getComponent<TransformComponent>(m_playerID);
		auto& ps = _world.getComponent<SphereComponent>(m_playerID);
		auto& pm = _world.getComponent<MotionComponent>(m_playerID);

		Vector3 playerPos = { pt.position.x, pt.position.y, pt.position.z };

		for (auto& id : m_entities) {
			if (id == m_playerID) continue;

			auto& ot = _world.getComponent<TransformComponent>(id);
			auto& os = _world.getComponent<SphereComponent>(id);

			Vector3 otherPos = { ot.position.x, ot.position.y, ot.position.z };

			if (CheckCollisionSpheres(playerPos, ps.radius, otherPos, os.radius)) {
				m_playerDead = true;
				pm.velocity = { 0, 0, 0 };
				EventBus::getInstance().publish(ee::event::PlayerDeadEvent{});
				EventBus::getInstance().publish(ee::event::AlertSoundEvent{});
				return;
			}
		}
	}
};
