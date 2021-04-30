#pragma once

namespace mth {
	float lerp(float a, float b, float t) {
		return a + (b - a) * t;
	}
}