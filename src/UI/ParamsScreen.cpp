#include "juicy-game/UI/ParamsScreen.hpp"
#include "juicy-game/UI/UIWidgetBasic.hpp"
#include <raylib.h>


ParamsScreen::ParamsScreen()
{
	float cx   = GetScreenWidth()  * 0.5f;
	float cy   = GetScreenHeight() * 0.5f;
	Font  font = GetFontDefault();

	// TODO: ajouter sliders volume, qualité graphique, etc.

	addWidget(new UIButton(
		{ cx - 100, cy + 160 }, { 200, 50 },
		[]() { UIManager::getInstance().pop(); },
		"Retour", font, 24.f
	));
}

void ParamsScreen::OnDraw(ee::renderer::Renderer3D& _renderer)
{
	DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), { 0, 0, 0, 180 });
	DrawText("Parametres", (int)(GetScreenWidth() * 0.5f) - 90, 80, 40, WHITE);
	for (auto* w : m_widgets) w->OnDraw(_renderer);
}
