#pragma once
#include "juicy-game/Utils.hpp"

#include "MySystem.hpp"

#include "../Components/TransformComponent.hpp"
#include "../Components/SphereComponent.hpp"
#include "../Components/MotionComponent.hpp"
#include "../Components/ProjectileComponent.hpp"
#include "../Components/PlayerComponent.hpp"

#include "../Input/ActionMap.hpp"
#include "../Input/InputManager.hpp"
#include <SDL2/SDL.h>  // constantes de binding uniquement (SDL_SCANCODE_*, SDL_CONTROLLER_*)

#include "juicy-game/EventsBus.hpp"
#include "../Events/CameraShakeEvent.hpp"
#include "../Events/SoundsEvent.hpp"
#include "../Events/TimeMultiplicatorEvent.hpp"

#include "raymath.h"
#include "rlgl.h"

#include <iostream>

using Quat = ee::math::Quaternion;
using V3 = ee::math::Vector3<float>;

enum class PlayerAction { Pitch, Bank, Forward, Boost, Shoot };

class PlayerSystem : public MySystem {


	static constexpr float BANK_ANGLE = 30.f;
	static constexpr float BANK_SPEED = 5.f;
	static constexpr float PITCH_SPEED = 50.f;
	static constexpr float YAW_SPEED = 50.f;
	static constexpr float BASE_SPEED = 0.f;
	static constexpr float ACCELERATION = 0.1f;
	static constexpr float PROJ_SPEED = 500.f;
	static constexpr float STABLE_LIMIT = 0.6;

	static constexpr int THRUSTER_MATERIAL_INDEX = 6;


	bool m_playerIsDead = false;
	float m_timeMultiplicator = 1.f;

	ActionMap<PlayerAction> m_actions;

	Color m_baseColor = { 0, 0, 0, 255 };
	Color m_speedColor = { 255, 23, 0, 255 };
	Color m_BoostColor = { 0, 186, 255, 255 };


public:

	void init() {
		EventBus::getInstance().subscribe<ee::event::TimeMultiplicatorEvent>(
			[this](ee::event::TimeMultiplicatorEvent _event) {
				m_timeMultiplicator = _event.amout;
			}
		);
		EventBus::getInstance().subscribe<ee::event::PlayerDeadEvent>(
			[this](ee::event::PlayerDeadEvent _event) {
				m_playerIsDead = true;
			}
		);

		// Manette — SDL2 normalise tous les contrôleurs (PS4/PS5/Switch/Xbox) via sa base de mapping.
		// Pas besoin de détecter le type : les constantes sont universelles.
		m_actions.bind(PlayerAction::Pitch,   {{ InputData::gamepadAxis(SDL_CONTROLLER_AXIS_LEFTY,        0.15f), TriggerState::Held }});
		m_actions.bind(PlayerAction::Bank,    {{ InputData::gamepadAxis(SDL_CONTROLLER_AXIS_LEFTX,        0.15f), TriggerState::Held }});
		m_actions.bind(PlayerAction::Forward, {{ InputData::gamepadAxis(SDL_CONTROLLER_AXIS_TRIGGERRIGHT, 0.1f),  TriggerState::Held }});
		m_actions.bind(PlayerAction::Boost,   {{ InputData::gamepadAxis(SDL_CONTROLLER_AXIS_TRIGGERLEFT,  0.1f),  TriggerState::Held }});
		m_actions.bind(PlayerAction::Shoot,   {{ InputData::controller(SDL_CONTROLLER_BUTTON_B),                  TriggerState::Down }});

		// Clavier fallback
		m_actions.bind(PlayerAction::Forward, {{ InputData::keyboard(SDL_SCANCODE_W),      TriggerState::Held }});
		m_actions.bind(PlayerAction::Boost,   {{ InputData::keyboard(SDL_SCANCODE_LSHIFT),  TriggerState::Held }});
		m_actions.bind(PlayerAction::Shoot,   {{ InputData::keyboard(SDL_SCANCODE_SPACE),   TriggerState::Down }});
		m_actions.bind(PlayerAction::Pitch,   {{ InputData::keyboard(SDL_SCANCODE_UP),      TriggerState::Held }});
		m_actions.bind(PlayerAction::Pitch,   {{ InputData::keyboard(SDL_SCANCODE_DOWN),    TriggerState::Held }});
		m_actions.bind(PlayerAction::Bank,    {{ InputData::keyboard(SDL_SCANCODE_LEFT),    TriggerState::Held }});
		m_actions.bind(PlayerAction::Bank,    {{ InputData::keyboard(SDL_SCANCODE_RIGHT),   TriggerState::Held }});
	}

	void update(ee::ecs::World& _world, float _dt) override {

		for (auto& id : m_entities) {

			auto& t = _world.getComponent<TransformComponent>(id);
			auto& s = _world.getComponent<SphereComponent>(id);
			auto& m = _world.getComponent<MotionComponent>(id);
			auto& p = _world.getComponent<PlayerComponent>(id);
			auto& mo = _world.getComponent<ModelComponent>(id);

			m.velocity = { 0, 0, 0 };

			auto axisOrDigital = [&](PlayerAction action, float digitalValue) -> float {
				float v = m_actions.getAxisValue(action);
				if (v != 0.f) return v;
				return m_actions.isActive(action) ? digitalValue : 0.f;
			};

			// Pitch : axe analogique OU touches haut/bas (digital signé)
			float pitchInput = m_actions.getAxisValue(PlayerAction::Pitch);
			if (pitchInput == 0.f && m_actions.isActive(PlayerAction::Pitch)) {
				if (InputManager::getInstance().checkState(InputData::keyboard(SDL_SCANCODE_UP),   TriggerState::Held)) pitchInput =  1.f;
				if (InputManager::getInstance().checkState(InputData::keyboard(SDL_SCANCODE_DOWN), TriggerState::Held)) pitchInput = -1.f;
			}

			// Bank : axe analogique OU touches gauche/droite
			float bankInput = -m_actions.getAxisValue(PlayerAction::Bank);
			if (bankInput == 0.f && m_actions.isActive(PlayerAction::Bank)) {
				if (InputManager::getInstance().checkState(InputData::keyboard(SDL_SCANCODE_LEFT),  TriggerState::Held)) bankInput =  1.f;
				if (InputManager::getInstance().checkState(InputData::keyboard(SDL_SCANCODE_RIGHT), TriggerState::Held)) bankInput = -1.f;
			}

			float rt   = max(0.f, axisOrDigital(PlayerAction::Forward, 1.f));
			float rl   = max(0.f, axisOrDigital(PlayerAction::Boost,   1.f));
			bool  shot = m_actions.isActive(PlayerAction::Shoot);

			float inputForward = (rt + rl > BASE_SPEED) ? rt + rl : BASE_SPEED;

			m.speed += GetMouseWheelMove() * 10;


			//Rotation
			if (pitchInput != 0.f) {
				auto delta = Quat::fromAxisAngle(t.orientation.right(), pitchInput * p.speedIntensity * PITCH_SPEED * _dt * DEG2RAD);
				t.orientation = (delta * t.orientation).Normalize();
			} 

			p.bankAngle += (bankInput * BANK_ANGLE - p.bankAngle) * BANK_SPEED  * _dt;

			float yawRate = p.bankAngle / BANK_ANGLE * p.speedIntensity * YAW_SPEED * 1.3f;
			auto yawDelta = Quat::fromAxisAngle(t.orientation.up(), yawRate * _dt * DEG2RAD);
			t.orientation = (yawDelta * t.orientation).Normalize();



			float target = inputForward;
			if (p.currentBoost <= 0)
				target = min(1.f, inputForward);

			//Accel deccel
			p.speedIntensity = Lerp(p.speedIntensity, target, powf(ACCELERATION * _dt, 0.75f));

			if (p.speedIntensity < 0.001f) p.speedIntensity = 0.f;

			m.velocity = t.orientation.forward() * p.speedIntensity;

			//Boost
			if (inputForward > 1.f)
				p.currentBoost -= 10 * _dt;
			else
				p.currentBoost += 5 * _dt;

			//cappage
			p.currentBoost = min(max(0.f, p.currentBoost), p.maxBoost);

			//feedBack reacteur
			mo.modelAsset->getModel().materials[THRUSTER_MATERIAL_INDEX].maps[MATERIAL_MAP_DIFFUSE].color = getColor(p.speedIntensity);

			if (m_playerIsDead == false) EventBus::getInstance().publish(ee::event::MoteurSoundEvent {p.speedIntensity, m_timeMultiplicator < 1.f ? 1 - m_timeMultiplicator : min(1.5f, p.speedIntensity)});

			if (p.speedIntensity >= STABLE_LIMIT) EventBus::getInstance().publish(ee::event::CameraShakeEvent {0.02f, p.speedIntensity - STABLE_LIMIT});

			InputManager::getInstance().rumble(max(0.f, p.speedIntensity - STABLE_LIMIT) / 2, max(0.f, p.speedIntensity - STABLE_LIMIT) / 2);


			if (shot) {
				auto eid = _world.createEntity();
				V3 spawnPos = t.position + t.orientation.forward() * (s.radius * 0.8f);
				V3 projVel = t.orientation.forward() + m.velocity * (m.speed / PROJ_SPEED);
				_world.addComponent(eid, TransformComponent{ spawnPos, t.orientation});
				_world.addComponent(eid, SphereComponent{ 10.f});
				_world.addComponent(eid, MotionComponent{ projVel, {0, 0, 0}, PROJ_SPEED, 0 });
				_world.addComponent(eid, ProjectileComponent{ 5.f, 1 });
				
				EventBus::getInstance().publish(ee::event::LaserSoundEvent {});
			}
		}
	}


private:

	Color getColor(float _t) {

		if (_t > 1) {
			_t = (_t - 1) / (2 - 1);
			return Color(m_speedColor.r * (1 - _t) + _t * m_BoostColor.r, m_speedColor.g * (1 - _t) + _t * m_BoostColor.g, m_speedColor.b * (1 - _t) + _t * m_BoostColor.b, 255);
		}

		return Color(m_baseColor.r * (1 - _t) + _t * m_speedColor.r, m_baseColor.g * (1 - _t) + _t * m_speedColor.g, m_baseColor.b * (1 - _t) + _t * m_speedColor.b, 255);
	}
};

