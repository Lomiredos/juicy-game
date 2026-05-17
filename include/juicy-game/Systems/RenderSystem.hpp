#pragma once

#include "MySystem.hpp"
#include "../Components/TransformComponent.hpp"
#include "Renderer3D.hpp"

class RenderSystem : public MySystem
{
public:
    void render(ee::ecs::World& _world, ee::renderer::Renderer3D& _renderer) override
    {
        _renderer.BeginScene();

        for (auto& id : m_entities)
        {
            auto& t = _world.getComponent<TransformComponent>(id);

            _renderer.PushMatrix();
            _renderer.Translate(t.position.x, t.position.y, t.position.z);
            _renderer.Rotate(t.rotation.x, 1, 0, 0);
            _renderer.Rotate(t.rotation.y, 0, 1, 0);
            _renderer.Rotate(t.rotation.z, 0, 0, 1);
            _renderer.DrawBox({ 0, 0, 0 }, { t.size.x, t.size.y, t.size.z }, RED);
            _renderer.DrawBoxWires({ 0, 0, 0 }, { t.size.x, t.size.y, t.size.z }, DARKGRAY);
            _renderer.PopMatrix();
        }

        _renderer.DrawGrid(50, 1.0f);
        _renderer.EndScene();
    }
};
