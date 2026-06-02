#pragma once

#include <raylib.h>
#include <cstdlib>


class ModelAsset {

private:

	Model m_model  = {};
	bool  m_loaded = false;

	ModelAsset() = default;
	ModelAsset(const char* _path);

	ModelAsset(const ModelAsset&)            = delete;
	ModelAsset& operator=(const ModelAsset&) = delete;

public:

	~ModelAsset();

	Model& getModel() { return m_model; }
	const Model& getModel() const { return m_model; }
	bool isLoaded()         const { return m_loaded; }

	void load(const char* _path);

	friend class ModelManager;
};
