#include "epch.h"
#include "Triangle.h"

bool Triangle::is_triangle(Triangle t) {
	if (t.points == points) {
		return true;
	}
	bool return_val{ false };
	int p{ 0 };
	bool barr[3] = { false, false, false };
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			if (points[i] == t.points[j]) {
				barr[p] = true;
				p++;
			}
		}
	}
	if (barr[0] && barr[1] && barr[2]) {
		return_val = true;
	}
	return return_val;
}

float Triangle::circle_rad() {
	float v_a = points[0]->returndistance(*points[1]);
	float v_b = points[1]->returndistance(*points[2]);
	float v_c = points[2]->returndistance(*points[0]);

	float A = Areal();

	return (v_a * v_b * v_c) / (4 * A);
}

float Triangle::Areal() {
	float v_a = points[0]->returndistance(*points[1]);
	float v_b = points[1]->returndistance(*points[2]);
	float v_c = points[2]->returndistance(*points[0]);

	float s = (v_a + v_b + v_c) / 2;

	return (sqrt)(s * (s - v_a) * (s - v_b) * (s - v_c));
}