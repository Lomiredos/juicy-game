#pragma once

#include <memory>
#include <vector>

#include "ecs/World.hpp"
#include "../Systems/MySystem.hpp"


namespace ee::renderer { class Renderer3D; }


namespace ee
{
    class Scene
    {

    protected:
        ee::ecs::World m_world;
        std::vector<std::shared_ptr<MySystem>> m_systems;
        std::vector<std::shared_ptr<MySystem>> m_UIsystems;

        Scene() = default;
        Scene(const Scene &) = delete;
        Scene &operator=(const Scene &) = delete;

    public:

        virtual ~Scene() = default;

        virtual void onEnter(ee::renderer::Renderer3D& _renderer) {}
        virtual void onExit() {}
        virtual void onUpdate() {}
        virtual void onRender(ee::renderer::Renderer3D& _renderer) {}
    };

}