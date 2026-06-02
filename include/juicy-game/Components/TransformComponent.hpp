#pragma once
#include "math/Vector3.hpp"
#include "math/Quaternion.hpp"

struct TransformComponent {
    ee::math::Vector3<float>  position;
    ee::math::Quaternion      orientation;
};
