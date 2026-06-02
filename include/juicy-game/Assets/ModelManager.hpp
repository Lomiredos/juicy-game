#pragma once

#include <memory>
#include <vector>
#include <cstring>

#include "juicy-game/Assets/ModelAsset.hpp"


class ModelManager {

	struct Entry {
		char                        name[128];
		std::shared_ptr<ModelAsset> asset;
	};

	std::vector<Entry> m_models;

	ModelManager() = default;
	ModelManager(const ModelManager&)            = delete;
	ModelManager& operator=(const ModelManager&) = delete;

public:

	static ModelManager& getInstance() {
		static ModelManager instance;
		return instance;
	}

	// Retourne nullptr si pas trouvé
	std::shared_ptr<ModelAsset> getModel(const char* _name);

	// Charge si absent, retourne le cache sinon
	std::shared_ptr<ModelAsset> getModel(const char* _name, const char* _path);

	void preload(const char* _name, const char* _path);

	~ModelManager() { m_models.clear(); }
};
