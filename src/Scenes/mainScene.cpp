#include "juicy-game/Scenes/mainScene.hpp"

#include "raylib.h"
#include "Renderer3D.hpp"

#include "juicy-game/UI/UIElem.hpp"
#include "juicy-game/UI/UIWidgetBasic.hpp"
#include "juicy-game/UI/ParamsScreen.hpp"


#include "juicy-game/Particles/ParticleSystem.hpp"


#include "juicy-game/EventsBus.hpp"
#include "juicy-game/Events/PlayerEvent.hpp"
#include "juicy-game/Events/CameraShakeEvent.hpp"
#include "juicy-game/Events/TimeMultiplicatorEvent.hpp"

#include "juicy-game/Systems/MotionSystem.hpp"
#include "juicy-game/Systems/RenderSystem.hpp"
#include "juicy-game/Systems/PlayerSystem.hpp"
#include "juicy-game/Systems/ProjectileSystem.hpp"
#include "juicy-game/Systems/CollisionSystem.hpp"
#include "juicy-game/Systems/CameraSystem.hpp"
#include "juicy-game/Systems/HUDSystem.hpp"
#include "juicy-game/Systems/PlanetSystem.hpp"
#include "juicy-game/Systems/ScoreSystem.hpp"
#include "juicy-game/Systems/SoundsSystem.hpp"
#include "juicy-game/Systems/MusicSystem.hpp"
#include "juicy-game/Input/InputManager.hpp"

#include "juicy-game/Components/TransformComponent.hpp"
#include "juicy-game/Components/PlayerComponent.hpp"
#include "juicy-game/Components/ProjectileComponent.hpp"
#include "juicy-game/Components/ModelComponent.hpp"
#include "juicy-game/Components/SphereComponent.hpp"
#include "juicy-game/Components/NoComponent.hpp"
#include "juicy-game/Components/DangerousComponent.hpp"


class PauseScreen : public UIScreen {

	std::function<void()> m_onResume;
	std::function<void()> m_onQuitToMenu;

public:

	PauseScreen(std::function<void()> _onResume, std::function<void()> _onQuitToMenu)
		: m_onResume(_onResume), m_onQuitToMenu(_onQuitToMenu)
	{
		float cx = GetScreenWidth() * 0.5f;
		float cy = GetScreenHeight() * 0.5f;
		Font  font = GetFontDefault();

		addWidget(new UIButton(
			{ cx - 100, cy - 40 }, { 200, 50 },
			[this]() { m_onResume(); },
			"Reprendre", font, 24.f
		));

		addWidget(new UIButton(
			{ cx - 100, cy + 30 }, { 200, 50 },
			[]() { UIManager::getInstance().push(new ParamsScreen()); },
			"Parametres", font, 24.f
		));

		addWidget(new UIButton(
			{ cx - 100, cy + 100 }, { 200, 50 },
			[this]() { m_onQuitToMenu(); },
			"Menu principal", font, 24.f
		));
	}

	void OnOpen()  override {}
	void OnClose() override {}

	void OnDraw(ee::renderer::Renderer3D& _renderer) override {
		DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), { 0, 0, 0, 160 });
		DrawText("Pause",
			(int)(GetScreenWidth() * 0.5f) - 55, 100, 60, WHITE);
		for (auto* w : m_widgets) w->OnDraw(_renderer);
	}
};

class GameOverScreen : public UIScreen {

	std::function<void()> m_onReplay;
	std::function<void()> m_onQuitToMenu;

public:

	GameOverScreen(std::function<void()> _onReplay, std::function<void()> _onQuitToMenu)
		: m_onReplay(_onReplay), m_onQuitToMenu(_onQuitToMenu)
	{
		float cx = GetScreenWidth() * 0.5f;
		float cy = GetScreenHeight() * 0.5f;
		Font  font = GetFontDefault();

		addWidget(new UIButton(
			{ cx - 100, cy + 20 }, { 200, 50 },
			[this]() { m_onReplay(); },
			"Rejouer", font, 24.f
		));

		addWidget(new UIButton(
			{ cx - 100, cy + 90 }, { 200, 50 },
			[this]() { m_onQuitToMenu(); },
			"Menu principal", font, 24.f
		));
	}

	void OnOpen()  override {}
	void OnClose() override {}

	void OnDraw(ee::renderer::Renderer3D& _renderer) override {
		DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), { 0, 0, 0, 160 });
		DrawText("Game Over",
			(int)(GetScreenWidth() * 0.5f) - 110, 100, 60, RED);
		for (auto* w : m_widgets) w->OnDraw(_renderer);
	}
};


void mainScene::onEnter(ee::renderer::Renderer3D& _renderer)
{
	m_renderer = &_renderer;
	m_dying = false;
	m_explosing = false;
	m_explosingTimer = 0.f;
	m_deathFlashAlpha = 0.f;

	ParticleSystem::getInstance().init();

	// mort
	EventBus::getInstance().subscribe<ee::event::PlayerDeadEvent>(
		[this](ee::event::PlayerDeadEvent e) {

			if (m_dying) return;
			m_dying = true;

			EventBus::getInstance().publish(ee::event::CameraShakeEvent { 0.4f, 1.5f });

			InputManager::getInstance().rumble(0.5f, 0.7f, 10);
		}
	);

	EventBus::getInstance().subscribe<ee::event::PlayerExploseEvent>(
		[this](ee::event::PlayerExploseEvent e) {

			if (m_explosing) return;
			m_explosing = true;

			EventBus::getInstance().publish(ee::event::CameraShakeEvent { 3.f, 1.5f });

			InputManager::getInstance().rumble(1.f, 1.f, 10);
		}
	);

	EventBus::getInstance().subscribe<ee::event::TimeMultiplicatorEvent>(
		[this](ee::event::TimeMultiplicatorEvent e) {
			m_timeMultiplicator = e.amout;
		}
	);

	initFirstEntity(_renderer);
	SetUpSystem(_renderer);
	m_world.flush();
}

void mainScene::onExit()
{
	while (UIManager::getInstance().getCurrentScreen())
		UIManager::getInstance().pop();
	ParticleSystem::getInstance().unload();
}

void mainScene::triggerGameOver()
{
	if (UIManager::getInstance().getCurrentScreen()) return;

	InputManager::getInstance().rumble(0.f, 0.f, 0);
	UIManager::getInstance().push(new GameOverScreen(
		[this]() { m_pendingAction = SceneAction::Replay;    },
		[this]() { m_pendingAction = SceneAction::GoToMenu;  }
	));

}

void mainScene::handlePendingAction()
{
	SceneAction action = m_pendingAction;
	m_pendingAction = SceneAction::None;

	EventBus::getInstance().clearAll();

	switch (action) {

	case SceneAction::GoToMenu:
		while (UIManager::getInstance().getCurrentScreen())
			UIManager::getInstance().pop();
		m_sm.setCurrentScene(m_menuId, *m_renderer);
		break;

	case SceneAction::Replay: {
		ee::SceneId id = m_sm.getCurrentSceneId();
		m_sm.replaceScene(id, std::make_unique<mainScene>(m_sm, m_menuId));
		m_sm.setCurrentScene(id, *m_renderer);
		break;
	}

	default: break;
	}
}

void mainScene::onUpdate()
{
	if (m_pendingAction != SceneAction::None) {
		handlePendingAction();
		return;
	}

	float dt = GetFrameTime() * m_timeMultiplicator;

	InputManager::getInstance().update();

	ParticleSystem::getInstance().update(dt);

	if (m_dying && !m_explosing) {

		auto& pT = m_world.getComponent<TransformComponent>(m_playerID);
		Vector3 pp = { pT.position.x, pT.position.y, pT.position.z };

		ParticleSystem::getInstance().burst(pp, 80, 900.f, GRAY, 9.f, 1.8f);
		ParticleSystem::getInstance().burst(pp, 50, 450.f, WHITE, 4.f, 1.0f);
		ParticleSystem::getInstance().burst(pp, 30, 200.f, BLACK, 6.f, 2.0f);
	}

	if (m_explosing) {
		m_explosingTimer += dt;
		
		m_deathFlashAlpha = 255 * (m_explosingTimer / EXPLOSION_TIME);
		if (m_deathFlashAlpha > 255) m_deathFlashAlpha = 255;

		if (m_explosingTimer > EXPLOSION_TIME * 1.5f)
			triggerGameOver();

		auto& pT = m_world.getComponent<TransformComponent>(m_playerID);
		Vector3 pp = { pT.position.x, pT.position.y, pT.position.z };

		ParticleSystem::getInstance().burst(pp, 80, 900.f, ORANGE, 9.f, 1.8f);
		ParticleSystem::getInstance().burst(pp, 50, 450.f, WHITE, 4.f, 1.0f);
		ParticleSystem::getInstance().burst(pp, 30, 200.f, RED, 6.f, 2.0f);

	}


	if (!UIManager::getInstance().getCurrentScreen() && !m_explosing) {

		for (auto& system : m_systems)
			system->update(m_world, dt);

		for (auto& system : m_UIsystems)
			system->update(m_world, dt);

		m_world.flush();

		if (IsKeyPressed(KEY_F1)) m_showDebug = !m_showDebug;

		if (IsKeyPressed(KEY_ESCAPE))
			UIManager::getInstance().push(new PauseScreen(
				[]() { UIManager::getInstance().pop(); },
				[this]() { m_pendingAction = SceneAction::GoToMenu; }
		));
	}
	else if(UIManager::getInstance().getCurrentScreen()) {
		UIManager::getInstance().update(dt);
	}
}

void mainScene::onRender(ee::renderer::Renderer3D& _renderer)
{
	_renderer.BeginFrame();
	_renderer.BeginScene();

	for (auto& system : m_systems)
		system->render(m_world, _renderer, m_showDebug);

	ParticleSystem::getInstance().render(_renderer.GetCamera());

	_renderer.EndScene();

	if (m_explosing)
		DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
			{ 0, 0, 0, (unsigned char)m_deathFlashAlpha  });


	if (!UIManager::getInstance().getCurrentScreen()) {
		for (auto& system : m_UIsystems)
			system->render(m_world, _renderer, m_showDebug);
	}
	UIManager::getInstance().render(_renderer);

	_renderer.EndFrame();
}


void mainScene::initFirstEntity(ee::renderer::Renderer3D& _renderer)
{
	m_playerID = m_world.createEntity();
	auto first = m_playerID;
	m_world.addComponent(first, TransformComponent{ {0, 0, 0}, ee::math::Quaternion::identity() });
	m_world.addComponent(first, SphereComponent{ 8.f });
	m_world.addComponent(first, PlayerComponent{ 0.f, 0.f, 100.f, 100.f });
	m_world.addComponent(first, MotionComponent{ {0, 0, 0}, {0, 0, 0}, 1500, 50 });
	ModelComponent comp;
	comp.init("spaceship", "assets/spaceShip.glb");
	comp.rotOffset = 270.f;
	m_world.addComponent(first, comp);
}

void mainScene::SetUpSystem(ee::renderer::Renderer3D& _renderer)
{
	ee::ecs::Signature sig;

	auto render = m_world.registerSystem<RenderSystem>();
	m_systems.push_back(render);
	render->priority = 10;
	render->init();
	sig.reset();
	sig.set(ee::ecs::getComponentID<TransformComponent>());
	sig.set(ee::ecs::getComponentID<SphereComponent>());
	m_world.setSystemSignature<RenderSystem>(sig);

	auto collide = m_world.registerSystem<CollisionSystem>();
	m_systems.push_back(collide);
	collide->priority = 200;
	collide->init(m_playerID);
	sig.reset();
	sig.set(ee::ecs::getComponentID<TransformComponent>());
	sig.set(ee::ecs::getComponentID<SphereComponent>());
	sig.set(ee::ecs::getComponentID<DangerousComponent>());
	m_world.setSystemSignature<CollisionSystem>(sig);

	auto planets = m_world.registerSystem<PlanetSystem>();
	m_systems.push_back(planets);
	planets->priority = 45;
	planets->init(_renderer, render->getFogShader());
	sig.reset();
	sig.set(ee::ecs::getComponentID<PlayerComponent>());
	sig.set(ee::ecs::getComponentID<TransformComponent>());
	m_world.setSystemSignature<PlanetSystem>(sig);

	auto motion = m_world.registerSystem<MotionSystem>();
	m_systems.push_back(motion);
	motion->priority = 110;
	sig.reset();
	sig.set(ee::ecs::getComponentID<TransformComponent>());
	sig.set(ee::ecs::getComponentID<MotionComponent>());
	m_world.setSystemSignature<MotionSystem>(sig);

	auto playersys = m_world.registerSystem<PlayerSystem>();
	m_systems.push_back(playersys);
	playersys->priority = 100;
	playersys->init();
	sig.reset();
	sig.set(ee::ecs::getComponentID<TransformComponent>());
	sig.set(ee::ecs::getComponentID<PlayerComponent>());
	sig.set(ee::ecs::getComponentID<MotionComponent>());
	sig.set(ee::ecs::getComponentID<ModelComponent>());
	m_world.setSystemSignature<PlayerSystem>(sig);

	auto proj = m_world.registerSystem<ProjectileSystem>();
	m_systems.push_back(proj);
	proj->priority = 51;
	sig.reset();
	sig.set(ee::ecs::getComponentID<TransformComponent>());
	sig.set(ee::ecs::getComponentID<MotionComponent>());
	sig.set(ee::ecs::getComponentID<ProjectileComponent>());
	m_world.setSystemSignature<ProjectileSystem>(sig);

	auto cam = m_world.registerSystem<CameraSystem>();
	m_systems.push_back(cam);
	cam->priority = 115;
	cam->init(_renderer);
	sig.reset();
	sig.set(ee::ecs::getComponentID<PlayerComponent>());
	sig.set(ee::ecs::getComponentID<TransformComponent>());
	m_world.setSystemSignature<CameraSystem>(sig);

	auto HUD = m_world.registerSystem<HUDSystem>();
	HUD->init();
	m_UIsystems.push_back(HUD);
	HUD->priority = 1000;
	sig.reset();
	sig.set(ee::ecs::getComponentID<PlayerComponent>());
	m_world.setSystemSignature<HUDSystem>(sig);

	auto score = m_world.registerSystem<ScoreSystem>();
	m_systems.push_back(score);
	score->priority = 90;
	score->init(m_playerID);
	sig.reset();
	sig.set(ee::ecs::getComponentID<TransformComponent>());
	sig.set(ee::ecs::getComponentID<DangerousComponent>());
	sig.set(ee::ecs::getComponentID<SphereComponent>());
	m_world.setSystemSignature<ScoreSystem>(sig);

	auto sound = m_world.registerSystem<SoundsSystem>();
	m_systems.push_back(sound);
	sound->priority = 100;
	sound->init();
	sig.reset();
	sig.set(ee::ecs::getComponentID<NoComponent>());
	m_world.setSystemSignature<SoundsSystem>(sig);

	auto music = m_world.registerSystem<MusicSystem>();
	m_UIsystems.push_back(music);
	music->priority = 999;
	music->init();
	sig.reset();
	sig.set(ee::ecs::getComponentID<NoComponent>());
	m_world.setSystemSignature<MusicSystem>(sig);

	std::sort(m_UIsystems.begin(), m_UIsystems.end(),
		[](const auto& a, const auto& b) { return a->priority < b->priority; });

	std::sort(m_systems.begin(), m_systems.end(),
		[](const auto& a, const auto& b) { return a->priority < b->priority; });
}
