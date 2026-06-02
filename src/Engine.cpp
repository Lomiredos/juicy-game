#include "juicy-game/Engine.hpp"
#include "juicy-game/Scenes/mainScene.hpp"
#include "juicy-game/Scenes/MenuScene.hpp"
#include "juicy-game/Input/InputManager.hpp"
#include "juicy-game/Input/ControllerSelectScreen.hpp"
#include <SDL2/SDL.h>

void Engine::Start(const char* _name, int _width, int _height, int _targetFPS)
{
	SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER);
	m_renderer.Init(_width, _height, _name, _targetFPS);

	auto gameScene    = std::make_unique<mainScene>(m_sceneManager, 0 );
	mainScene* gamePtr = gameScene.get();

	ee::SceneId gameId = m_sceneManager.addScene(std::move(gameScene));
	ee::SceneId menuId = m_sceneManager.addScene(
		std::make_unique<MenuScene>(m_sceneManager, gameId));

	gamePtr->setMenuId(menuId);

	m_sceneManager.setCurrentScene(menuId, m_renderer);

	InputManager::getInstance().refreshControllers();
	SelectController();
}

void Engine::Run()
{
	while (!WindowShouldClose())
	{
		m_sceneManager.flushPending();
		Update();
		if (!WindowShouldClose())
			Render();
	}
}

void Engine::Render()
{
	m_sceneManager.getCurrentScene().onRender(m_renderer);
}

void Engine::Update()
{
	m_sceneManager.getCurrentScene().onUpdate();
}

void Engine::Quit()
{
	if (IsWindowReady())
		CloseWindow();
}
