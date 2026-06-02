#pragma once

#include "Renderer3D.hpp"
#include "Scenes/SceneManager.hpp"

class Engine
{
private:
    ee::renderer::Renderer3D m_renderer;
    ee::SceneManager m_sceneManager;

public:
    void Start(const char* _name, int _width, int _height, int _targetFPS = 60);
    void Run();
    void Quit();

private:
    
    void Render();
    void Update();
};
