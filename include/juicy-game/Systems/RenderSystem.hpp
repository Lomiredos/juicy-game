#pragma once

#include "MySystem.hpp"
#include "../Components/TransformComponent.hpp"

#include "raylib.h"
#include "rlgl.h"

class RenderSystem : public MySystem {


public:
	
	void render(ee::ecs::World& _world, Camera3D& _cam) override {
		

		BeginMode3D(_cam);

		
		for (auto& id : m_entities) {
			auto& transformComp = _world.getComponent<TransformComponent>(id);
			rlPushMatrix();
			rlTranslatef(transformComp.position.x, transformComp.position.y, transformComp.position.z);
			DrawCube({ 0, 0, 0 }, transformComp.size.x, transformComp.size.y, transformComp.size.z, RED);
			rlPopMatrix();
		}


		EndMode3D();
	}
};