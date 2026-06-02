#pragma once
#include <unordered_map>
#include "Scene.hpp"


namespace ee::renderer { class Renderer3D; }


namespace ee
{
	using SceneId = size_t;

	class SceneManager
	{

	private:
		std::unordered_map<SceneId, std::unique_ptr<ee::Scene>> m_scenes;
		std::unique_ptr<ee::Scene> m_pendingDestroy;
		SceneId m_currentSceneID = 0;

	public:
		SceneId addScene(std::unique_ptr<Scene> _scene)
		{
			SceneId id = m_scenes.size();
			m_scenes[id] = std::move(_scene);
			return id;
		}

		void setCurrentScene(SceneId _id, ee::renderer::Renderer3D& _renderer)
		{
			if (m_scenes.count(_id) == false) return;
			m_scenes[m_currentSceneID]->onExit();
			m_currentSceneID = _id;
			m_scenes[m_currentSceneID]->onEnter(_renderer);

		}

		SceneId getCurrentSceneId() const
		{
			return m_currentSceneID;
		}

		Scene& getCurrentScene()
		{
			return *m_scenes[m_currentSceneID];
		}

		void replaceScene(SceneId _id, std::unique_ptr<Scene> _scene)
		{
			if (m_scenes.count(_id)) {
				m_pendingDestroy = std::move(m_scenes[_id]);
				m_scenes[_id] = std::move(_scene);
			}
		}

		void flushPending()
		{
			m_pendingDestroy.reset();
		}

		~SceneManager()
		{
			m_scenes.clear();
		}
	};
}