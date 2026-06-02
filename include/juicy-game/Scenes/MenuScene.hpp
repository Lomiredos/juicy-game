#pragma once
#include "Scene.hpp"
#include "SceneManager.hpp"


class MenuScene : public ee::Scene {

	ee::SceneManager&         m_sm;
	ee::SceneId               m_gameId;
	ee::renderer::Renderer3D* m_renderer = nullptr;

public:

	MenuScene(ee::SceneManager& _sm, ee::SceneId _gameId)
		: m_sm(_sm), m_gameId(_gameId) {}

	void onEnter(ee::renderer::Renderer3D& _renderer) override;
	void onExit()  override;
	void onUpdate() override;
	void onRender(ee::renderer::Renderer3D& _renderer) override;
};
