#pragma once

#include "MySystem.hpp"


#include "../Components/PlayerComponent.hpp"

#include "rlgl.h";

#include <string>

class HUDSystem : public MySystem {

	Font m_font;

public:

	void init() {
		m_font = LoadFontEx("assets/Fonts/space.otf", 32, 0, 250);
	}

	void render(ee::ecs::World& _world, ee::renderer::Renderer3D& _renderer, bool _showDebug) override {

		float maxBoost = 1.f, currentBoost = 0.f, speedIntensity = 0.f;
		float score = 0.f, multiplier = 1.f;

		for (auto& id : m_entities) {
			auto& p = _world.getComponent<PlayerComponent>(id);
			maxBoost      = p.maxBoost;
			currentBoost  = p.currentBoost;
			speedIntensity = p.speedIntensity;
			score          = p.score;
			multiplier     = p.multiplier;
		}

		// ── Jauge boost ────────────────────────────────────────────────────────
		_renderer.DrawRing({ 100, 500 }, 85, 90, -80 + 210 - 210 * currentBoost / maxBoost, 130, 50, RED);
		_renderer.DrawRingLine({ 100, 500 }, 85, 90, -80, 130, 50, GRAY);

		const char* speedTxt = TextFormat("%d", (int)(speedIntensity * 100));
		Vector2 speedSize = MeasureTextEx(m_font, speedTxt, 32, 5);
		_renderer.DrawText(m_font, speedTxt, { 100 - speedSize.x * 0.5f, 500 - speedSize.y * 0.5f }, 32, 5, WHITE);

		// ── Score ──────────────────────────────────────────────────────────────
		const char* scoreTxt = TextFormat("%d", (int)score);
		Vector2 scoreSize = MeasureTextEx(m_font, scoreTxt, 48, 4);
		float   cx        = GetScreenWidth() * 0.5f;
		_renderer.DrawText(m_font, scoreTxt, { cx - scoreSize.x * 0.5f, 20.f }, 48, 4, WHITE);

		// ── Multiplicateur ─────────────────────────────────────────────────────
		Color multColor = WHITE;
		if      (multiplier >= 6.f) multColor = RED;
		else if (multiplier >= 3.f) multColor = ORANGE;
		else if (multiplier >= 2.f) multColor = YELLOW;

		const char* multTxt  = TextFormat("x%.1f", multiplier);
		Vector2     multSize = MeasureTextEx(m_font, multTxt, 32, 4);
		_renderer.DrawText(m_font, multTxt, { cx - multSize.x * 0.5f, 76.f }, 32, 4, multColor);
	}
};