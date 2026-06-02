#pragma once

#include "KeysBinding.hpp"
#include "InputManager.hpp"

#include <unordered_map>
#include <algorithm>


template <typename T>
class ActionMap
{

private:

    std::unordered_map<T, std::vector<KeyBinding>> m_actions;

    bool checkInput(const std::pair<InputData, TriggerState>& key) const
    {
        return InputManager::getInstance().checkState(key.first, key.second);
    }

public:

    void bind(T _action, const std::vector<std::pair<InputData, TriggerState>>& _cond)
    {
        KeyBinding kb;
        for (const auto& pair : _cond)
            kb.inputs.push_back(pair);
        m_actions[_action].push_back(kb);
    }

    void rebind(T _action, const std::vector<std::pair<InputData, TriggerState>>& _cond)
    {
        if (!m_actions.contains(_action)) return;
        KeyBinding kb;
        for (const auto& pair : _cond)
            kb.inputs.push_back(pair);
        m_actions[_action] = { kb };
    }



    bool isActive(T _action) const
    {
        if (!m_actions.contains(_action)) return false;

        for (const auto& kb : m_actions.at(_action)) {
            bool valide = true;
            for (const auto& duo : kb.inputs)
                valide &= checkInput(duo);
            if (valide)
                return true;
        }
        return false;
    }

    float getAxisValue(T _action) const
    {
        if (!m_actions.contains(_action)) return 0.f;

        for (const auto& kb : m_actions.at(_action)) {
            for (const auto& duo : kb.inputs) {
                if (duo.first.type == InputType::GamepadAxis ||
                    duo.first.type == InputType::MouseAxis)
                    return InputManager::getInstance().getAxisValue(duo.first);
            }
        }
        return 0.f;
    }
};
