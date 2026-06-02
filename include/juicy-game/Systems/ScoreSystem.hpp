#pragma once

#include "MySystem.hpp"
#include "../Components/TransformComponent.hpp"
#include "../Components/SphereComponent.hpp"
#include "../Components/PlayerComponent.hpp"

#include "raymath.h"


#include "../EventsBus.hpp"
#include "../Events/TimeMultiplicatorEvent.hpp"


class ScoreSystem : public MySystem {

	ee::ecs::EntityID m_playerID;
	bool m_timeChanged = false;


	static constexpr float BASE_RATE   = 100.f;
	static constexpr float PROX_RADIUS = 450.f; 

public:

	void init(ee::ecs::EntityID _playerId) { m_playerID = _playerId; }

	void update(ee::ecs::World& _world, float _dt) override
	{
		auto& pt = _world.getComponent<TransformComponent>(m_playerID);
		auto& pp = _world.getComponent<PlayerComponent>(m_playerID);

		Vector3 playerPos = { pt.position.x, pt.position.y, pt.position.z };

		std::vector<float> dists;
		float minDist = INTMAX_MAX;
		for (auto& id : m_entities) {
			if (id == m_playerID) continue;
			auto& ot = _world.getComponent<TransformComponent>(id);
			auto& os = _world.getComponent<SphereComponent>(id);
			Vector3 op   = { ot.position.x, ot.position.y, ot.position.z };
			float   dist = Vector3Distance(playerPos, op) - os.radius;
			if (dist < minDist) minDist = dist;
			if (dist < PROX_RADIUS) dists.push_back(dist);

		}

		if (minDist < PROX_RADIUS / 5.f && m_timeChanged == false) {
			EventBus::getInstance().publish(ee::event::TimeMultiplicatorEvent {0.3f});
			m_timeChanged = true;
		}
		if (minDist > PROX_RADIUS / 5.f && m_timeChanged) {
			EventBus::getInstance().publish(ee::event::TimeMultiplicatorEvent {1.f});
			m_timeChanged = false;
		}

		float proxMult = 1.f;
		for (float dist : dists) {
			proxMult += Clamp(1.f - dist / PROX_RADIUS, 0.f, 10.f);
		}

		float speedMult = pp.speedIntensity * 3.f;

		pp.multiplier = proxMult * speedMult;
		pp.score     += _dt * pp.multiplier * BASE_RATE;
	}
};
