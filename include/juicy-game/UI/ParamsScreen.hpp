#pragma once
#include "juicy-game/UI/UIElem.hpp"


// Partagé entre MenuScene et la pause
class ParamsScreen : public UIScreen {
public:
	ParamsScreen();
	void OnOpen()  override {}
	void OnClose() override {}
	void OnDraw(ee::renderer::Renderer3D& _renderer) override;
};
