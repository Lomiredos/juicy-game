#pragma once

#include <memory>
#include "juicy-game/Assets/ModelAsset.hpp"
#include "juicy-game/Assets/ModelManager.hpp"


struct ModelComponent {
	std::shared_ptr<ModelAsset> modelAsset;
	float   rotOffset  = 0.f;
	Vector3 modelScale = { 1.f, 1.f, 1.f };

	void init(const char* _name, const char* _path) {
		modelAsset = ModelManager::getInstance().getModel(_name, _path);
	}
};
