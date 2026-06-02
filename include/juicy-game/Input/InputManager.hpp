#pragma once
#include "KeysBinding.hpp"

class InputManager
{
public:
    static InputManager& getInstance();

    void update();

    bool  checkState(const InputData& input, TriggerState state) const;
    float getAxisValue(const InputData& input) const;

    bool        isControllerAvailable() const;
    const char* getControllerName()     const;

    int         getControllerCount()                       const;
    const char* getControllerName(int slot)                const;
    bool        anyControllerInput(int slot)               const;
    bool        controllerButtonDown(int slot, int sdlBtn) const;
    void        setPrimaryController(int slot); // -1 = auto (premier dispo)
    void        rumble(float left, float right, int durationMs = 50);
    int         getControllerType(int slot)                const; // SDL_GameControllerType
    void        refreshControllers(); // à appeler avant le premier update() pour peupler les slots

private:
    InputManager();
    ~InputManager();

    InputManager(const InputManager&)            = delete;
    InputManager& operator=(const InputManager&) = delete;
};
