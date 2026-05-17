#pragma once
#include <memory>
#include <vector>

#include "ecs/World.hpp"
#include "juicy-game/Systems/MySystem.hpp"

#include "math/Vector3.hpp"

#include "raylib.h"

class Engine {
private:
	ee::ecs::World m_world;
	std::vector<std::shared_ptr<MySystem>> m_systems;

	Camera3D m_camera;

	ee::math::Vector3<float> m_cubePos = { 0, 0, 0 };

public:
	void Start(const char* _name, int _width, int _height, int _targetFPS = 60);
	void Run();
	void Quit();

private:

	void initFirstEntity();
	void SetUpSystem();
	void Render();
	void Update();

};