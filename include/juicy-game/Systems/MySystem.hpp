#pragma once

#include "ecs/System.hpp"


struct Camera3D;

class MySystem : public ee::ecs::System {

public:
	int priority = 0;
	virtual void update(ee::ecs::World& _world, float _dt) {};

	virtual void render(ee::ecs::World& _world, Camera3D& _cam) {};
};