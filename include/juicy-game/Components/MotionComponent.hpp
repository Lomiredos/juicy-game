#pragma once

#include "math/Vector3.hpp"

struct MotionComponent {

	ee::math::Vector3<float> velocity;
	ee::math::Vector3<float> angularVelocity;
	ee::math::Vector3<float> speed;
	ee::math::Vector3<float> angularSpeed;
};