#pragma once
#include "Scene.hpp"
#include "SceneManager.hpp"


class mainScene : public ee::Scene {

	ee::SceneManager&         m_sm;
	ee::SceneId               m_menuId;
	ee::renderer::Renderer3D* m_renderer = nullptr;

	ee::ecs::EntityID m_playerID = 0;
	bool m_showDebug = false;

	float m_timeMultiplicator = 1.f;


	// Séquence de mort
	bool  m_explosing       = false;
	bool  m_dying           = false;
	float m_explosingTimer  = 0.f;
	float m_deathFlashAlpha = 0.f;

	enum class SceneAction { None, GoToMenu, Replay };
	SceneAction m_pendingAction = SceneAction::None;

	static constexpr float EXPLOSION_TIME = 1.5f; 

public:

	mainScene(ee::SceneManager& _sm, ee::SceneId _menuId)
		: m_sm(_sm), m_menuId(_menuId) {}

	void setMenuId(ee::SceneId _id) { m_menuId = _id; }

	void onEnter(ee::renderer::Renderer3D& _renderer) override;
	void onExit()  override;
	void onUpdate() override;
	void onRender(ee::renderer::Renderer3D& _renderer) override;


	void triggerGameOver();

private:

	void initFirstEntity(ee::renderer::Renderer3D& _renderer);
	void SetUpSystem(ee::renderer::Renderer3D& _renderer);
	void handlePendingAction();
};
