#pragma once

#include <vector>

enum class InputType  { Mouse, KeyBoard, Controller, GamepadAxis, MouseAxis };
enum class TriggerState { Down, Held, Released };

struct InputData {
    InputType type;
    int       value;
    float     deadzone = 0.2f;

    // Les valeurs passées ici sont les int des enums SDL2 — pas besoin d'inclure SDL2 ici.
    static InputData keyboard   (int key,  float dz = 0.f)  { return { InputType::KeyBoard,    key, dz }; }
    static InputData mouse      (int btn,  float dz = 0.f)  { return { InputType::Mouse,       btn, dz }; }
    static InputData controller (int btn,  float dz = 0.f)  { return { InputType::Controller,  btn, dz }; }
    static InputData gamepadAxis(int axis, float dz = 0.2f) { return { InputType::GamepadAxis, axis, dz }; }
    static InputData mouseAxis  (int axis, float dz = 1.f)  { return { InputType::MouseAxis,   axis, dz }; }
};

struct KeyBinding {
    std::vector<std::pair<InputData, TriggerState>> inputs;
};
