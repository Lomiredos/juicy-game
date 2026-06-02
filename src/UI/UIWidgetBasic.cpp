#include "juicy-game/UI/UIWidgetBasic.hpp"

#include <raylib.h>
#include <cmath>
#include <algorithm>


static void drawTextureFit(Texture2D tex, float x, float y, float w, float h)
{
	Rectangle src  = { 0, 0, (float)tex.width, (float)tex.height };
	Rectangle dest = { x, y, w, h };
	DrawTexturePro(tex, src, dest, { 0, 0 }, 0.f, WHITE);
}



void UILabel::OnDraw(ee::renderer::Renderer3D&)
{
	if (m_haveTexture && m_texture)
		drawTextureFit(m_texture->getTexture(), m_position.x, m_position.y, m_size.x, m_size.y);
	else if (m_text)
		DrawTextEx(m_font, m_text, { m_position.x, m_position.y }, m_fontSize, 1.f, m_color);
}


void UIButton::OnFocus()   { m_focused = true;  }
void UIButton::OnUnFocus() { m_focused = false; }
void UIButton::OnConfirm() { m_callBack(); }

void UIButton::OnDraw(ee::renderer::Renderer3D&)
{
	auto& tex = (m_focused && m_focusTexture) ? m_focusTexture : m_defaultTexture;

	if (tex)
		drawTextureFit(tex->getTexture(), m_position.x, m_position.y, m_size.x, m_size.y);
	else
		DrawRectangle((int)m_position.x, (int)m_position.y,
		              (int)m_size.x, (int)m_size.y,
		              m_focused ? BLUE : WHITE);

	if (m_hasText && m_text) {
		Vector2 ts = MeasureTextEx(m_font, m_text, m_fontSize, 1.f);
		DrawTextEx(m_font, m_text,
		           { m_position.x + (m_size.x - ts.x) * 0.5f,
		             m_position.y + (m_size.y - ts.y) * 0.5f },
		           m_fontSize, 1.f, { 129, 135, 145, 255 });
	}
}


void UICheckBox::OnFocus()   { m_focused = true;  }
void UICheckBox::OnUnFocus() { m_focused = false; }

void UICheckBox::OnConfirm()
{
	m_state = !m_state;
	m_callBack(m_state);
}

void UICheckBox::OnDraw(ee::renderer::Renderer3D&)
{
	auto& tex = (m_state && m_fullTexture) ? m_fullTexture : m_emptyTexture;

	if (tex) {
		drawTextureFit(tex->getTexture(), m_position.x, m_position.y, m_size.x, m_size.y);
	}
	else {
		DrawRectangle((int)m_position.x, (int)m_position.y,
		              (int)m_size.x, (int)m_size.y,
		              m_focused ? BLUE : WHITE);

		if (m_state) {
			DrawLine((int)m_position.x,             (int)m_position.y,
			         (int)(m_position.x + m_size.x), (int)(m_position.y + m_size.y), BLACK);
			DrawLine((int)(m_position.x + m_size.x), (int)m_position.y,
			         (int)m_position.x,              (int)(m_position.y + m_size.y), BLACK);
		}
	}

	if (m_hasText && m_text) {
		DrawTextEx(m_font, m_text,
		           { m_position.x + m_size.x * 1.5f,
		             m_position.y + (m_size.y - m_fontSize) * 0.5f },
		           m_fontSize, 1.f, { 129, 135, 145, 255 });
	}
}


void UISlider::OnFocus()   { m_focused = true;  }
void UISlider::OnUnFocus() { m_focused = false; }

void UISlider::OnConfirm()
{
	if (m_secure) m_confirm = !m_confirm;
}

void UISlider::OnDraw(ee::renderer::Renderer3D&)
{
	// Track
	if (m_trackTexture)
		drawTextureFit(m_trackTexture->getTexture(), m_position.x, m_position.y, m_size.x, m_size.y);
	else
		DrawRectangle((int)m_position.x, (int)m_position.y,
		              (int)m_size.x, (int)m_size.y, { 129, 135, 145, 255 });

	// Knob
	float coef    = (m_value - m_min) / (m_max - m_min);
	float knobDim = m_horizontal ? m_size.x / 10.f : m_size.y / 10.f;
	float knobX   = m_position.x + coef * (m_size.x - (m_horizontal ? knobDim : 0.f));
	float knobY   = m_position.y + coef * (m_size.y - (m_horizontal ? 0.f : knobDim));
	float knobW   = m_horizontal ? knobDim  : m_size.x;
	float knobH   = m_horizontal ? m_size.y : knobDim;

	if (m_knobTexture)
		drawTextureFit(m_knobTexture->getTexture(), knobX, knobY, knobW, knobH);
	else {
		Color knobColor = m_focused ? (m_secure && !m_confirm ? WHITE : BLUE) : WHITE;
		DrawRectangle((int)knobX, (int)knobY, (int)knobW, (int)knobH, knobColor);
	}

	// Label au-dessus
	if (m_hasLabel && m_label) {
		Vector2 ls = MeasureTextEx(m_font, m_label, m_fontSize, 1.f);
		DrawTextEx(m_font, m_label,
		           { m_position.x + (m_size.x - ls.x) * 0.5f,
		             m_position.y - ls.y * 1.5f },
		           m_fontSize, 1.f, { 129, 135, 145, 255 });
	}
}

bool UISlider::OnDirectional(ee::math::Vector2<float> _dir)
{
	if (m_secure && !m_confirm) return false;

	if (m_horizontal) {
		if (fabsf(_dir.y) > 0.9f) return false;
	}
	else {
		if (fabsf(_dir.x) > 0.9f) return false;
	}

	float delta = m_horizontal ? _dir.x : _dir.y;
	m_value += delta > 0.f ? m_step : -m_step;
	m_value  = std::clamp(m_value, m_min, m_max);
	m_callBack(m_value);
	return true;
}
