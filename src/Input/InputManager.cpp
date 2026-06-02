#include "juicy-game/Input/InputManager.hpp"

#include <SDL2/SDL.h>
#include <cstring>
#include <cmath>

namespace {

    // Clavier
    Uint8 g_currKeys[SDL_NUM_SCANCODES] = {};
    Uint8 g_prevKeys[SDL_NUM_SCANCODES] = {};

    // Souris
    Uint32 g_currMouse = 0;
    Uint32 g_prevMouse = 0;
    int    g_mouseDX   = 0;
    int    g_mouseDY   = 0;

    // Multi-manette
    static const int MAX_CTRL = 4;
    SDL_GameController* g_controllers[MAX_CTRL] = {};
    int                 g_primarySlot = -1;

    // Données de tous les pads
    Uint8  g_allCurrBtns[MAX_CTRL][SDL_CONTROLLER_BUTTON_MAX] = {};
    Sint16 g_allCurrAxes[MAX_CTRL][SDL_CONTROLLER_AXIS_MAX]   = {};

    // Manette primaire — état curr/prev pour checkState (rétro-compat)
    SDL_GameController* g_controller = nullptr;
    Uint8 g_currBtns[SDL_CONTROLLER_BUTTON_MAX] = {};
    Uint8 g_prevBtns[SDL_CONTROLLER_BUTTON_MAX] = {};

    void refreshAllControllers()
    {
        // Fermer les slots déconnectés
        for (int s = 0; s < MAX_CTRL; s++) {
            if (g_controllers[s] && !SDL_GameControllerGetAttached(g_controllers[s])) {
                SDL_GameControllerClose(g_controllers[s]);
                g_controllers[s] = nullptr;
                if (g_primarySlot == s)
                    g_primarySlot = -1;
            }
        }

        // Ouvrir les nouveaux joysticks dans les slots libres
        for (int i = 0; i < SDL_NumJoysticks(); i++) {
            if (!SDL_IsGameController(i)) continue;

            SDL_JoystickID id = SDL_JoystickGetDeviceInstanceID(i);
            if (id < 0) continue;

            bool alreadyOpen = false;
            for (int s = 0; s < MAX_CTRL; s++) {
                if (g_controllers[s] &&
                    SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(g_controllers[s])) == id) {
                    alreadyOpen = true;
                    break;
                }
            }
            if (alreadyOpen) continue;

            for (int s = 0; s < MAX_CTRL; s++) {
                if (!g_controllers[s]) {
                    g_controllers[s] = SDL_GameControllerOpen(i);
                    break;
                }
            }
        }

        // Si le slot primaire n'est plus valide, auto-sélection sur le premier dispo
        if (g_primarySlot < 0 || !g_controllers[g_primarySlot]) {
            g_primarySlot = -1;
            for (int s = 0; s < MAX_CTRL; s++) {
                if (g_controllers[s]) { g_primarySlot = s; break; }
            }
        }

        g_controller = (g_primarySlot >= 0) ? g_controllers[g_primarySlot] : nullptr;
    }

} // namespace


InputManager::InputManager()  = default;

InputManager::~InputManager()
{
    for (int s = 0; s < MAX_CTRL; s++) {
        if (g_controllers[s]) {
            SDL_GameControllerClose(g_controllers[s]);
            g_controllers[s] = nullptr;
        }
    }
    g_controller = nullptr;
}

InputManager& InputManager::getInstance()
{
    static InputManager instance;
    return instance;
}

void InputManager::update()
{
    SDL_PumpEvents();

    // Clavier
    memcpy(g_prevKeys, g_currKeys, SDL_NUM_SCANCODES);
    const Uint8* state = SDL_GetKeyboardState(nullptr);
    memcpy(g_currKeys, state, SDL_NUM_SCANCODES);

    // Souris
    g_prevMouse = g_currMouse;
    g_currMouse = SDL_GetMouseState(nullptr, nullptr);
    SDL_GetRelativeMouseState(&g_mouseDX, &g_mouseDY);

    // Manettes — poll tous les slots
    refreshAllControllers();

    for (int s = 0; s < MAX_CTRL; s++) {
        if (g_controllers[s]) {
            for (int b = 0; b < SDL_CONTROLLER_BUTTON_MAX; b++)
                g_allCurrBtns[s][b] = SDL_GameControllerGetButton(g_controllers[s], (SDL_GameControllerButton)b);
            for (int a = 0; a < SDL_CONTROLLER_AXIS_MAX; a++)
                g_allCurrAxes[s][a] = SDL_GameControllerGetAxis(g_controllers[s], (SDL_GameControllerAxis)a);
        } else {
            memset(g_allCurrBtns[s], 0, SDL_CONTROLLER_BUTTON_MAX);
            memset(g_allCurrAxes[s], 0, SDL_CONTROLLER_AXIS_MAX * sizeof(Sint16));
        }
    }

    // Mise à jour du pad primaire (curr/prev) pour checkState
    memcpy(g_prevBtns, g_currBtns, SDL_CONTROLLER_BUTTON_MAX);
    if (g_primarySlot >= 0)
        memcpy(g_currBtns, g_allCurrBtns[g_primarySlot], SDL_CONTROLLER_BUTTON_MAX);
    else
        memset(g_currBtns, 0, SDL_CONTROLLER_BUTTON_MAX);
}

bool InputManager::checkState(const InputData& input, TriggerState state) const
{
    switch (input.type)
    {
    case InputType::KeyBoard:
    {
        int sc = input.value;
        switch (state)
        {
        case TriggerState::Down:     return  g_currKeys[sc] && !g_prevKeys[sc];
        case TriggerState::Held:     return  g_currKeys[sc];
        case TriggerState::Released: return !g_currKeys[sc] &&  g_prevKeys[sc];
        }
        return false;
    }
    case InputType::Mouse:
    {
        Uint32 mask = SDL_BUTTON(input.value);
        switch (state)
        {
        case TriggerState::Down:     return  (g_currMouse & mask) && !(g_prevMouse & mask);
        case TriggerState::Held:     return  (g_currMouse & mask);
        case TriggerState::Released: return !(g_currMouse & mask) &&  (g_prevMouse & mask);
        }
        return false;
    }
    case InputType::Controller:
    {
        if (!g_controller) return false;
        int btn = input.value;
        switch (state)
        {
        case TriggerState::Down:     return  g_currBtns[btn] && !g_prevBtns[btn];
        case TriggerState::Held:     return  g_currBtns[btn];
        case TriggerState::Released: return !g_currBtns[btn] &&  g_prevBtns[btn];
        }
        return false;
    }
    case InputType::GamepadAxis:
        if (!g_controller) return false;
        return fabsf(getAxisValue(input)) > input.deadzone;

    case InputType::MouseAxis:
    {
        float v = input.value == 0 ? (float)g_mouseDX : (float)g_mouseDY;
        return fabsf(v) > input.deadzone;
    }
    default:
        return false;
    }
}

float InputManager::getAxisValue(const InputData& input) const
{
    switch (input.type)
    {
    case InputType::GamepadAxis:
        if (!g_controller) return 0.f;
        return SDL_GameControllerGetAxis(g_controller, (SDL_GameControllerAxis)input.value) / 32767.f;

    case InputType::MouseAxis:
        return input.value == 0 ? (float)g_mouseDX : (float)g_mouseDY;

    default:
        return 0.f;
    }
}

bool InputManager::isControllerAvailable() const
{
    return g_controller != nullptr;
}

const char* InputManager::getControllerName() const
{
    if (!g_controller) return nullptr;
    return SDL_GameControllerName(g_controller);
}

int InputManager::getControllerCount() const
{
    int count = 0;
    for (int s = 0; s < MAX_CTRL; s++)
        if (g_controllers[s]) count++;
    return count;
}

const char* InputManager::getControllerName(int slot) const
{
    if (slot < 0 || slot >= MAX_CTRL || !g_controllers[slot]) return nullptr;
    return SDL_GameControllerName(g_controllers[slot]);
}

bool InputManager::anyControllerInput(int slot) const
{
    if (slot < 0 || slot >= MAX_CTRL || !g_controllers[slot]) return false;
    for (int b = 0; b < SDL_CONTROLLER_BUTTON_MAX; b++)
        if (g_allCurrBtns[slot][b]) return true;
    for (int a = 0; a < SDL_CONTROLLER_AXIS_MAX; a++)
        if (fabsf(g_allCurrAxes[slot][a] / 32767.f) > 0.2f) return true;
    return false;
}

bool InputManager::controllerButtonDown(int slot, int sdlBtn) const
{
    if (slot < 0 || slot >= MAX_CTRL || !g_controllers[slot]) return false;
    if (sdlBtn < 0 || sdlBtn >= SDL_CONTROLLER_BUTTON_MAX) return false;
    return g_allCurrBtns[slot][sdlBtn] != 0;
}

void InputManager::rumble(float left, float right, int durationMs)
{
    if (!g_controller) return;
    SDL_GameControllerRumble(g_controller,
        (Uint16)(left  * 65535.f),
        (Uint16)(right * 65535.f),
        (Uint32)durationMs);
}

int InputManager::getControllerType(int slot) const
{
    if (slot < 0 || slot >= MAX_CTRL || !g_controllers[slot])
        return SDL_CONTROLLER_TYPE_UNKNOWN;
    return SDL_GameControllerGetType(g_controllers[slot]);
}

void InputManager::refreshControllers()
{
    refreshAllControllers();
}

void InputManager::setPrimaryController(int slot)
{
    if (slot < 0 || slot >= MAX_CTRL || !g_controllers[slot])
        g_primarySlot = -1;
    else
        g_primarySlot = slot;

    g_controller = (g_primarySlot >= 0) ? g_controllers[g_primarySlot] : nullptr;
}
