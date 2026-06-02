#pragma once

#include <vector>
#include "math/Vector2.hpp"
#include "Renderer3D.hpp"

class UIwidget {
protected:
	ee::math::Vector2<float> m_position;
	ee::math::Vector2<float> m_size;
	bool m_focused;


public:
	
	UIwidget(ee::math::Vector2<float> _pos, ee::math::Vector2<float> _size) : m_position(_pos), m_size(_size), m_focused(false) {}

	virtual void OnFocus() = 0;
	virtual void OnUnFocus() = 0;
	virtual void OnConfirm() = 0;
	virtual void OnDraw(ee::renderer::Renderer3D& _renderer) = 0;

	virtual bool OnDirectional(ee::math::Vector2<float> _direction) { return false; }
	virtual bool isFocusable() const { return true; }

	ee::math::Vector2<float> getPos(float _anchorX = 0.5f, float _anchorY = 0.5f) const { return ee::math::Vector2<float>(m_position.x + m_size.x * _anchorX, m_position.y + m_size.y * _anchorY); }


};


class UIScreen {
protected:
	std::vector<UIwidget*> m_widgets;
	UIwidget* m_currentWidget = nullptr;
	float m_lastChange = 0;
	float m_changeInter = 0.2f;

public:

	UIwidget* getCurrentWidget() const { return m_currentWidget; }
    
	virtual void OnOpen() = 0;
	virtual void OnClose() = 0;
	virtual void OnDraw(ee::renderer::Renderer3D& _renderer) = 0;

	// Appelé par UIManager avant sa gestion d'input. Retourne true pour bloquer UIManager ce frame.
	virtual bool OnUpdate(float dt) { return false; }

	void navigateTo(ee::math::Vector2<float>  _direction);
	void addWidget(UIwidget* _widget);

	void AddTime(float _dt) { m_lastChange += _dt; }

	virtual ~UIScreen() {
		for (UIwidget* w : m_widgets) {
			delete w;
		}
	}
private:
	UIwidget* findNearest(ee::math::Vector2<float>  _dir);
};


class UIManager {

private:

	std::vector<UIScreen*> m_screens;



	UIManager() = default;
	UIManager(const UIManager&) = delete;
	UIManager& operator=(const UIManager&) = delete;

public:

	static UIManager& getInstance() {
		static UIManager instance;
		return instance;
	}
	
	UIScreen* getCurrentScreen() const { return m_screens.empty() ? nullptr : m_screens.back(); }

	void push(UIScreen* _screen);
	void pop();

	void update(float _dt);
	void render(ee::renderer::Renderer3D& _renderer);

};