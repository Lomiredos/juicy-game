#pragma once
#include "math/Vector3.hpp"

struct TransformComponent {
	ee::math::Vector3<float> position;
	ee::math::Vector3<float> size;
	ee::math::Vector3<float> rotation;
	ee::math::Vector3<float> scale;
};