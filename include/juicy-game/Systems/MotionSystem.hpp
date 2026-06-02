#pragma once
#include "MySystem.hpp"
#include "../Components/MotionComponent.hpp"
#include "../Components/TransformComponent.hpp"

#include "Renderer3D.hpp"
#include "math/Quaternion.hpp"

class MotionSystem : public MySystem {

	float m_dt;

public:

	void update(ee::ecs::World& _world, float _dt) {

		m_dt = _dt;
		for (auto& id : m_entities) {

			auto& t = _world.getComponent<TransformComponent>(id);
			auto& m = _world.getComponent<MotionComponent>(id);

			t.position += m.velocity * m.speed * _dt;
			auto delta =  ee::math::Quaternion::fromAxisAngle(m.angularVelocity, m.angularSpeed * _dt);
			t.orientation = (delta * t.orientation).Normalize();
		}
	}

	void render(ee::ecs::World& _world, ee::renderer::Renderer3D& _renderer, bool _showDebug) {

		if (!_showDebug) return;

		for (auto& id : m_entities) {

			auto& t = _world.getComponent<TransformComponent>(id);
			auto& m = _world.getComponent<MotionComponent>(id);




			for (int i = 0; i < 10; i++) {
				ee::math::Vector3<float> nextPos = t.position + m.velocity * m.speed * (m_dt * i);
				_renderer.DrawBox({ nextPos.x, nextPos.y, nextPos.z }, { 1, 1, 1 }, RED);
			}
			for (int i = 0; i > -10; i--) {
				ee::math::Vector3<float> nextPos = t.position + m.velocity * m.speed * (m_dt * i);
				_renderer.DrawBox({ nextPos.x, nextPos.y, nextPos.z }, { 1, 1, 1 }, GREEN);
			}
		}
	}
};


