#pragma once

#include <functional>
#include <memory>
#include <algorithm>

#include "juicy-game/UI/UIElem.hpp"
#include "juicy-game/Assets/Texture.hpp"
#include <raylib.h>


class UILabel : public UIwidget {
	const char* m_text     = nullptr;
	Font        m_font     = {};
	Color       m_color    = WHITE;
	float       m_fontSize = 20.f;

	std::shared_ptr<TextureAsset> m_texture;
	bool m_haveTexture = false;

public:

	// Label texte
	UILabel(ee::math::Vector2<float> _pos, ee::math::Vector2<float> _size,
	        const char* _text, Font _font, Color _color, float _fontSize = 20.f)
		: UIwidget(_pos, _size),
		  m_text(_text), m_font(_font), m_color(_color), m_fontSize(_fontSize) {}


	// Label texture
	UILabel(ee::math::Vector2<float> _pos, ee::math::Vector2<float> _size,
	        std::shared_ptr<TextureAsset> _texture)
		: UIwidget(_pos, _size), m_texture(_texture), m_haveTexture(true) {}

	void OnFocus()   override {}
	void OnUnFocus() override {}
	void OnConfirm() override {}
	void OnDraw(ee::renderer::Renderer3D& _renderer) override;
	bool isFocusable() const override { return false; }
};


class UIButton : public UIwidget {
	std::function<void()> m_callBack;

	const char* m_text     = nullptr;
	Font        m_font     = {};
	float       m_fontSize = 20.f;
	bool        m_hasText  = false;

	std::shared_ptr<TextureAsset> m_defaultTexture;
	std::shared_ptr<TextureAsset> m_focusTexture;

public:

	// Texture uniquement
	UIButton(ee::math::Vector2<float> _pos, ee::math::Vector2<float> _size,
	         std::function<void()> _func,
	         std::shared_ptr<TextureAsset> _default = nullptr,
	         std::shared_ptr<TextureAsset> _focus   = nullptr)
		: UIwidget(_pos, _size), m_callBack(_func),
		  m_defaultTexture(_default), m_focusTexture(_focus) {}

	// Texte + texture optionnelle
	UIButton(ee::math::Vector2<float> _pos, ee::math::Vector2<float> _size,
	         std::function<void()> _func,
	         const char* _text, Font _font, float _fontSize = 20.f,
	         std::shared_ptr<TextureAsset> _default = nullptr,
	         std::shared_ptr<TextureAsset> _focus   = nullptr)
		: UIwidget(_pos, _size), m_callBack(_func),
		  m_text(_text), m_font(_font), m_fontSize(_fontSize), m_hasText(true),
		  m_defaultTexture(_default), m_focusTexture(_focus) {}

	void OnFocus()   override;
	void OnUnFocus() override;
	void OnConfirm() override;
	void OnDraw(ee::renderer::Renderer3D& _renderer) override;
};


class UICheckBox : public UIwidget {
	bool m_state;
	std::function<void(bool)> m_callBack;

	const char* m_text     = nullptr;
	Font        m_font     = {};
	float       m_fontSize = 20.f;
	bool        m_hasText  = false;

	std::shared_ptr<TextureAsset> m_emptyTexture;
	std::shared_ptr<TextureAsset> m_fullTexture;

public:
	UICheckBox(ee::math::Vector2<float> _pos, ee::math::Vector2<float> _size,
	           std::function<void(bool)> _func, bool _state,
	           const char* _text = nullptr, Font _font = {}, float _fontSize = 20.f,
	           std::shared_ptr<TextureAsset> _empty = nullptr,
	           std::shared_ptr<TextureAsset> _full  = nullptr)
		: UIwidget(_pos, _size), m_callBack(_func), m_state(_state),
		  m_text(_text), m_font(_font), m_fontSize(_fontSize),
		  m_hasText(_text != nullptr),
		  m_emptyTexture(_empty), m_fullTexture(_full) {}

	void OnFocus()   override;
	void OnUnFocus() override;
	void OnConfirm() override;
	void OnDraw(ee::renderer::Renderer3D& _renderer) override;
};


class UISlider : public UIwidget {
	bool  m_secure;
	bool  m_confirm    = false;
	bool  m_horizontal = true;

	float m_min, m_max, m_value, m_step;

	std::function<void(float)> m_callBack;

	const char* m_label    = nullptr;
	Font        m_font     = {};
	float       m_fontSize = 20.f;
	bool        m_hasLabel = false;

	std::shared_ptr<TextureAsset> m_trackTexture;
	std::shared_ptr<TextureAsset> m_knobTexture;

public:

	// Sans label
	UISlider(ee::math::Vector2<float> _pos, ee::math::Vector2<float> _size,
	         std::function<void(float)> _func,
	         float _min, float _max, float _value, float _step, bool _secure,
	         std::shared_ptr<TextureAsset> _track = nullptr,
	         std::shared_ptr<TextureAsset> _knob  = nullptr)
		: UIwidget(_pos, _size), m_callBack(_func),
		  m_min(_min), m_max(_max), m_value(_value), m_step(_step),
		  m_secure(_secure), m_horizontal(_size.x > _size.y),
		  m_trackTexture(_track), m_knobTexture(_knob) {}

	// Avec label
	UISlider(ee::math::Vector2<float> _pos, ee::math::Vector2<float> _size,
	         std::function<void(float)> _func,
	         float _min, float _max, float _value, float _step, bool _secure,
	         const char* _label, Font _font, float _fontSize = 20.f,
	         std::shared_ptr<TextureAsset> _track = nullptr,
	         std::shared_ptr<TextureAsset> _knob  = nullptr)
		: UIwidget(_pos, _size), m_callBack(_func),
		  m_min(_min), m_max(_max), m_value(_value), m_step(_step),
		  m_secure(_secure), m_horizontal(_size.x > _size.y),
		  m_label(_label), m_font(_font), m_fontSize(_fontSize), m_hasLabel(true),
		  m_trackTexture(_track), m_knobTexture(_knob) {}

	void OnFocus()   override;
	void OnUnFocus() override;
	void OnConfirm() override;
	void OnDraw(ee::renderer::Renderer3D& _renderer) override;
	bool OnDirectional(ee::math::Vector2<float> _dir) override;
};
