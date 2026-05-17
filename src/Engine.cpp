#include "juicy-game/Engine.hpp"
#include "juicy-game/Systems/RenderSystem.hpp"
#include "juicy-game/Components/TransformComponent.hpp"

#include "raylib.h"

void Engine::Start(const char* _name, int _width, int _height, int _targetFPS)
{
    m_renderer.Init(_width, _height, _name, _targetFPS);

    SetUpSystem();
    initFirstEntity();
}

void Engine::Run()
{
    while (!WindowShouldClose())
    {
        Update();
        Render();
    }
}

void Engine::Render()
{
    m_renderer.BeginFrame();

    for (auto& system : m_systems)
        system->render(m_world, m_renderer);

    m_renderer.EndFrame();
}

void Engine::Update()
{
    float dt = GetFrameTime();
    for (auto& system : m_systems)
        system->update(m_world, dt);
}

void Engine::Quit()
{
    CloseWindow();
}

void Engine::initFirstEntity()
{
    auto first = m_world.createEntity();
    m_world.addComponent(first, TransformComponent{ {0, 0, 0}, {2.f, 2.f, 2.f}, {0, 0, 0}, {1, 1, 1} });
}

void Engine::SetUpSystem()
{
    ee::ecs::Signature sig;

    m_systems.push_back(m_world.registerSystem<RenderSystem>());
    m_systems.back()->priority = 100;
    sig.reset();
    sig.set(ee::ecs::getComponentID<TransformComponent>());
    m_world.setSystemSignature<RenderSystem>(sig);

    std::sort(m_systems.begin(), m_systems.end(),
        [](const auto& _a, const auto& _b) { return _a->priority < _b->priority; });
}
