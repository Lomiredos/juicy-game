#pragma once

template<typename T>
float max(const T _a, const T _b) {
	return _a > _b ? _a : _b;
}

template<typename T>
float min(const T _a, const T _b) {
	return _a < _b ? _a : _b;
}
