#pragma once

#include <raylib.h>
#include <cstdlib>


class TextureAsset {

private:

	Texture2D m_texture    = {};
	Image     m_image      = {};
	bool      m_imageReady = false;

	float m_width  = 0;
	float m_height = 0;

	TextureAsset(const char* _path);
	TextureAsset() = default;

	TextureAsset(const TextureAsset&)            = delete;
	TextureAsset& operator=(const TextureAsset&) = delete;

public:

	~TextureAsset();

	const Texture2D& getTexture() const { return m_texture; }

	float getHeight() const { return m_height; }
	float getWidth()  const { return m_width;  }

	void loadFromDisk(const char* _path);
	void uploadToGPU();

private:

	friend class AssetsManager;
	friend class Renderer;
};
