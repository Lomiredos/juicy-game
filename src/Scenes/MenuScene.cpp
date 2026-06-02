#include "juicy-game/Scenes/MenuScene.hpp"
#include "juicy-game/UI/UIElem.hpp"
#include "juicy-game/UI/UIWidgetBasic.hpp"
#include "juicy-game/UI/ParamsScreen.hpp"
#include "juicy-game/EventsBus.hpp"
#include "juicy-game/Input/InputManager.hpp"
#include <raylib.h>


class MainMenuScreen : public UIScreen {

	ee::SceneManager&         m_sm;
	ee::SceneId               m_gameId;
	ee::renderer::Renderer3D& m_renderer;

public:

	MainMenuScreen(ee::SceneManager& _sm, ee::SceneId _gameId, ee::renderer::Renderer3D& _renderer)
		: m_sm(_sm), m_gameId(_gameId), m_renderer(_renderer)
	{
		float cx   = GetScreenWidth()  * 0.5f;
		float cy   = GetScreenHeight() * 0.5f;
		Font  font = GetFontDefault();

		addWidget(new UIButton(
			{ cx - 100, cy - 80 }, { 200, 50 },
			[this]() { m_sm.setCurrentScene(m_gameId, m_renderer); },
			"Jouer", font, 28.f
		));

		addWidget(new UIButton(
			{ cx - 100, cy - 10 }, { 200, 50 },
			[]() { UIManager::getInstance().push(new ParamsScreen()); },
			"Parametres", font, 24.f
		));

		addWidget(new UIButton(
			{ cx - 100, cy + 60 }, { 200, 50 },
			[]() { UIManager::getInstance().pop(); },
			"Quitter", font, 24.f
		));

		
		
	}

	void OnOpen()  override {}
	void OnClose() override {}

	void OnDraw(ee::renderer::Renderer3D& _renderer) override {
		DrawText("Juicy Game",
		         (int)(GetScreenWidth() * 0.5f) - 110, 100, 60, WHITE);
		for (auto* w : m_widgets) w->OnDraw(_renderer);
	}
};



void MenuScene::onEnter(ee::renderer::Renderer3D& _renderer)
{
	m_renderer = &_renderer;
	UIManager::getInstance().push(new MainMenuScreen(m_sm, m_gameId, _renderer));
}

void MenuScene::onExit()
{
	while (UIManager::getInstance().getCurrentScreen())
		UIManager::getInstance().pop();
	EventBus::getInstance().clearAll();
}

void MenuScene::onUpdate()
{
	if (!UIManager::getInstance().getCurrentScreen())
		CloseWindow();
	InputManager::getInstance().update();
	UIManager::getInstance().update(GetFrameTime());
}

void MenuScene::onRender(ee::renderer::Renderer3D& _renderer)
{
	_renderer.BeginFrame();
	UIManager::getInstance().render(_renderer);
	_renderer.EndFrame();
}
