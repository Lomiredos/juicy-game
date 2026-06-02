#pragma once

struct PlayerComponent {
	float speedIntensity;
	float bankAngle;
	float maxBoost, currentBoost;

	float score      = 0.f;
	float multiplier = 1.f;
};