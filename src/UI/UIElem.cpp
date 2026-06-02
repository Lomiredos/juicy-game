#include "juicy-game/UI/UIElem.hpp"
#include "juicy-game/Input/InputManager.hpp"

#include <SDL2/SDL.h>
#include <raylib.h>
#include <cmath>
#include <cfloat>



void UIManager::push(UIScreen* _screen)
{
	m_screens.push_back(_screen);
	_screen->OnOpen();
}

void UIManager::pop()
{
	if (m_screens.empty()) return;
	m_screens.back()->OnClose();
	delete m_screens.back();
	m_screens.pop_back();
}

void UIManager::update(float _dt)
{
	if (m_screens.empty()) return;

	UIScreen* screen = m_screens.back();
	screen->AddTime(_dt);

	if (screen->OnUpdate(_dt)) return;

	if (!screen->getCurrentWidget()) return;

	auto& im = InputManager::getInstance();

	// Confirmer
	if (im.checkState(InputData::keyboard(SDL_SCANCODE_RETURN), TriggerState::Down) ||
	    im.checkState(InputData::controller(SDL_CONTROLLER_BUTTON_A), TriggerState::Down)) {
		screen->getCurrentWidget()->OnConfirm();
		return; // screen peut être invalide après un changement de scène
	}
 
	// Direction — gamepad en priorité, clavier en fallback
	ee::math::Vector2<float> dir = { 0.f, 0.f };
	float axisX = im.getAxisValue(InputData::gamepadAxis(SDL_CONTROLLER_AXIS_LEFTX, 0.f));
	float axisY = im.getAxisValue(InputData::gamepadAxis(SDL_CONTROLLER_AXIS_LEFTY, 0.f));

	if (axisX * axisX + axisY * axisY > 0.0625f) { // deadzone 0.25
		float len = sqrtf(axisX * axisX + axisY * axisY);
		dir = { axisX / len, axisY / len };
	}
	else if (im.checkState(InputData::keyboard(SDL_SCANCODE_UP),    TriggerState::Held)) dir = {  0.f, -1.f };
	else if (im.checkState(InputData::keyboard(SDL_SCANCODE_DOWN),  TriggerState::Held)) dir = {  0.f,  1.f };
	else if (im.checkState(InputData::keyboard(SDL_SCANCODE_LEFT),  TriggerState::Held)) dir = { -1.f,  0.f };
	else if (im.checkState(InputData::keyboard(SDL_SCANCODE_RIGHT), TriggerState::Held)) dir = {  1.f,  0.f };

	if (dir.x != 0.f || dir.y != 0.f) {
		if (!screen->getCurrentWidget()->OnDirectional(dir))
			screen->navigateTo(dir);
	}
}

void UIManager::render(ee::renderer::Renderer3D& _renderer)
{
	if (m_screens.empty())
		return;
	m_screens.back()->OnDraw(_renderer);
}


void UIScreen::addWidget(UIwidget* _widget)
{
	m_widgets.push_back(_widget);
	if (!m_currentWidget && _widget->isFocusable()) {
		m_currentWidget = _widget;
		_widget->OnFocus();
	}
}

void UIScreen::navigateTo(ee::math::Vector2<float> _dir)
{
	if (m_lastChange < m_changeInter) return;

	if (UIwidget* wid = findNearest(_dir)) {
		m_currentWidget->OnUnFocus();
		m_currentWidget = wid;
		m_currentWidget->OnFocus();
		m_lastChange = 0.f;
	}
}

UIwidget* UIScreen::findNearest(ee::math::Vector2<float> _dir)
{
	if (!m_currentWidget) return nullptr;

	ee::math::Vector2<float> focusPos = m_currentWidget->getPos();
	float     minDist = FLT_MAX;
	UIwidget* fav     = nullptr;

	for (UIwidget* widget : m_widgets) {
		if (widget == m_currentWidget) continue;
		if (!widget->isFocusable())    continue;

		ee::math::Vector2<float> candidatePos = widget->getPos();
		ee::math::Vector2<float> delta        = candidatePos - focusPos;

		if (delta.Dot(_dir) < 0.3f) continue;

		if (float dist = focusPos.Distance(candidatePos); dist < minDist) {
			minDist = dist;
			fav     = widget;
		}
	}
	return fav;
}
