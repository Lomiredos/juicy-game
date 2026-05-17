#include "juicy-game/Engine.hpp"


#include "juicy-game/Systems/RenderSystem.hpp"


#include "juicy-game/Components/TransformComponent.hpp"

void Engine::Start(const char* _name, int _width, int _height, int _targetFPS)
{
	InitWindow(_width, _height, _name);
	SetTargetFPS(_targetFPS);

	m_camera = {} ;
	m_camera.position = { 5.0f, 5.0f, 5.0f };
	m_camera.target = { 0.0f, 0.0f, 0.0f };
	m_camera.up = { 0.0f, 1.0f, 0.0f };
	m_camera.fovy = 45.0f;
	m_camera.projection = CAMERA_PERSPECTIVE;
	
	initFirstEntity();
	SetUpSystem();
}


void Engine::Run()
{

	while (!WindowShouldClose())
	{
		Update();
		Render();

		/*
		m_camera->target = m_cubePos;
		if (IsKeyDown(KEY_Z)) m_cubePos.z += speed * GetFrameTime();
		if (IsKeyDown(KEY_S)) m_cubePos.z -= speed * GetFrameTime();


		if (IsKeyDown(KEY_Q)) m_cubePos.x += speed * GetFrameTime();
		if (IsKeyDown(KEY_D)) m_cubePos.x -= speed * GetFrameTime();
		*/
	}
}


void Engine::Render()
{
	BeginDrawing();
	ClearBackground(RAYWHITE);


	for (auto& system : m_systems) {
		system->render(m_world, m_camera);
	}

	EndDrawing();
}

void Engine::Update()
{
	float dt = GetFrameTime();
	for (auto& system : m_systems) {
		system->update(m_world, dt);
	}
}

void Engine::Quit()
{

	CloseWindow();

}


void Engine::initFirstEntity()
{
	auto first = m_world.createEntity();
	m_world.addComponent(first, TransformComponent{ {0, 0, 0}, {2., 2., 2.}, {0, 0, 0}, {1, 1, 1}});
}

void Engine::SetUpSystem()
{
	ee::ecs::Signature sig;


	m_systems.push_back(m_world.registerSystem<RenderSystem>());
	m_systems.back()->priority = 100;
	sig.reset();
	sig.set(ee::ecs::getComponentID<TransformComponent>());
	m_world.setSystemSignature<RenderSystem>(sig);

















	//tri des system par ordre de prio
	std::sort(m_systems.begin(), m_systems.end(),
		[](const auto& _a, const auto& _b) { return _a->priority < _b->priority; });
}