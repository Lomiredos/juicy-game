#pragma once

#include "MySystem.hpp"

#include "../Components/TransformComponent.hpp"
#include "../Components/PlayerComponent.hpp"

#include "Renderer3D.hpp"
#include "../EventsBus.hpp"
#include "../Events/CameraShakeEvent.hpp"
#include "../Events/PlayerEvent.hpp"


using V3 = ee::math::Vector3<float>;

class CameraSystem : public MySystem {


	bool m_isPlayerDead = false;
	float m_delta = 0;
	float m_dt = 0;
	static constexpr float MAXOFFSET = 7.5f;
	static constexpr float CAM_SPEED = 10.f;

	ee::renderer::Renderer3D* m_renderer = nullptr;

	std::vector<ee::event::CameraShakeEvent> m_cameraShake;

public:
	void init(ee::renderer::Renderer3D& _renderer) { m_renderer = &_renderer; }

	CameraSystem() {
		EventBus::getInstance().subscribe<ee::event::CameraShakeEvent>(

			[this](ee::event::CameraShakeEvent _event) -> void
			{
				m_cameraShake.push_back(_event);
				std::sort(m_cameraShake.begin(), m_cameraShake.end(),
					[](const auto& _a, const auto& _b) {return _a.intensity > _b.intensity;});
			}
		);

		EventBus::getInstance().subscribe<ee::event::PlayerDeadEvent>(
			[this](ee::event::PlayerDeadEvent e) {
				m_isPlayerDead = true;
			}
		);
	}


	void update(ee::ecs::World& _world, float _dt) {

		for (int i = 0; i < m_cameraShake.size(); i++) {
			m_cameraShake[i].duration -= _dt;
		}

		m_dt = _dt;
		if (m_isPlayerDead) {
			m_delta += _dt;
			if (m_delta > 1.f) m_delta = 1.f;
		}

		m_cameraShake.erase(
			std::remove_if(m_cameraShake.begin(), m_cameraShake.end(),
				[](const auto& _e) {return _e.duration <= 0.f;}),
			m_cameraShake.end()
		);

		if (!m_renderer) return;

		for (auto& id : m_entities) {

			auto& t = _world.getComponent<TransformComponent>(id);
			auto& p = _world.getComponent<PlayerComponent>(id);

			V3 back = t.orientation.back();
			V3 up = t.orientation.up();

			float awayMult = 1.f;
			if (m_isPlayerDead) {
				awayMult = Lerp(awayMult, 8.f, powf(m_delta, 0.75f));
				if (awayMult >= 7.6f)
					EventBus::getInstance().publish(ee::event::PlayerExploseEvent {});
			}

			float fovy = Lerp(45.f, 75.f, p.speedIntensity);
			float backDist = (40.f * tanf(22.5f * DEG2RAD) / tanf(fovy * 0.5f * DEG2RAD));

			V3 camPos = t.position + back * max(50.f, p.speedIntensity * 40) * awayMult + up * max(30.f, p.speedIntensity * 30) * awayMult;
			V3 target = t.position + t.orientation.forward() * 15;

			float xOffSet = 0;
			float yOffSet = 0;

			if (m_cameraShake.size() > 0) {
				float intensity = m_cameraShake.front().intensity;
				xOffSet = GetRandomValue(-MAXOFFSET, MAXOFFSET) * intensity;
				yOffSet = GetRandomValue(-MAXOFFSET, MAXOFFSET) * intensity;
			}

			Camera3D& cam = m_renderer->GetCamera();
			cam.position = { camPos.x + xOffSet, camPos.y + yOffSet, camPos.z };
			cam.target = { target.x + xOffSet, target.y + yOffSet, target.z };
			cam.up = { up.x, up.y, up.z };
			cam.fovy = fovy;
		}
	}



};