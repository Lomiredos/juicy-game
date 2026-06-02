#include "juicy-game/Input/ControllerSelectScreen.hpp"
#include "juicy-game/Input/InputManager.hpp"
#include "juicy-game/UI/UIElem.hpp"

#include <SDL2/SDL.h>
#include <raylib.h>
#include <cmath>


class ControllerSelectScreen : public UIScreen
{
    int m_focusedSlot = -1;

public:

    void OnOpen()  override { m_focusedSlot = -1; }
    void OnClose() override {}
    void OnDraw(ee::renderer::Renderer3D&) override;
    bool OnUpdate(float _dt) override;
};

bool ControllerSelectScreen::OnUpdate(float)
{
    auto& im   = InputManager::getInstance();
    int   count = im.getControllerCount();

    for (int i = 0; i < count; i++) {

        if (!im.anyControllerInput(i)) continue;

        if (i == m_focusedSlot &&
            im.controllerButtonDown(i, SDL_CONTROLLER_BUTTON_A)) {
            im.setPrimaryController(i);
            UIManager::getInstance().pop();
            return true;
        }

        m_focusedSlot = i;
       
    }

    return true; // bloque UIManager pendant la sélection
}

void ControllerSelectScreen::OnDraw(ee::renderer::Renderer3D& /*renderer*/)
{
    auto& im = InputManager::getInstance();

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    DrawRectangle(0, 0, screenW, screenH, { 0, 0, 0, 180 });

    const int cardW = 420;
    const int cardH = 160;
    int cardX = (screenW - cardW) / 2;
    int cardY = (screenH - cardH) / 2;

    DrawRectangleRounded({ (float)cardX, (float)cardY, (float)cardW, (float)cardH },
                         0.15f, 8, { 30, 30, 30, 230 });
    DrawRectangleRoundedLines({ (float)cardX, (float)cardY, (float)cardW, (float)cardH },
                              0.15f, 8, WHITE);

    // Titre
    const char* title = "Selection manette";
    int titleSize = 20;
    int titleW = MeasureText(title, titleSize);
    DrawText(title, cardX + (cardW - titleW) / 2, cardY + 16, titleSize, LIGHTGRAY);

    // Nom du pad focusé
    const char* padName = (m_focusedSlot >= 0) ? im.getControllerName(m_focusedSlot)
                                                : nullptr;
    const char* padText = padName ? padName : "Appuyer sur n'importe quel boutton...";
    int padSize = 22;
    int padW = MeasureText(padText, padSize);
    Color padColor = padName ? WHITE : GRAY;
    DrawText(padText, cardX + (cardW - padW) / 2, cardY + 60, padSize, padColor);

    // Instruction
    const char* hint = (m_focusedSlot >= 0) ? "Appuyez sur A pour confirmer"
                                             : "";
    int hintSize = 16;
    int hintW = MeasureText(hint, hintSize);
    DrawText(hint, cardX + (cardW - hintW) / 2, cardY + 110, hintSize, GREEN);
}

void SelectController()
{
    auto& im = InputManager::getInstance();

    if (im.getControllerCount() <= 1) {
        im.setPrimaryController(0);
        return;
    }

    UIManager::getInstance().push(new ControllerSelectScreen());
}
