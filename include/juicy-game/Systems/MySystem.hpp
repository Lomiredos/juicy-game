#pragma once

#include "ecs/System.hpp"

namespace ee::renderer { class Renderer3D; }

class MySystem : public ee::ecs::System
{
public:
    int priority = 0;
    virtual void update(ee::ecs::World& _world, float _dt) {};
    virtual void render(ee::ecs::World& _world, ee::renderer::Renderer3D& _renderer) {};
};
