#pragma once
#include <memory>
#include <vector>

#include "ecs/World.hpp"
#include "juicy-game/Systems/MySystem.hpp"
#include "Renderer3D.hpp"

class Engine
{
private:
    ee::ecs::World m_world;
    std::vector<std::shared_ptr<MySystem>> m_systems;
    ee::renderer::Renderer3D m_renderer;

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
