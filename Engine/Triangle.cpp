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

	float m_a, m_b, m_c;
	float vec_a[2], vec_b[2], vec_c[2], p_a[2], p_b[2], p_c[2];
	
	p_a[0] = points[0]->getx();
	p_a[1] = points[0]->getz();
	p_b[0] = points[1]->getx();
	p_b[1] = points[1]->getz();
	p_c[0] = points[2]->getx();
	p_c[1] = points[2]->getz();

	vec_a[0] = p_a[0] - p_b[0];
	vec_a[1] = p_a[1] - p_b[1];

	vec_b[0] = p_b[0] - p_c[0];
	vec_b[1] = p_b[1] - p_c[1];

	vec_c[0] = p_c[0] - p_a[0];
	vec_c[1] = p_c[1] - p_a[1];

	float m_pa[2], m_pb[2], m_pc[2];

	m_pa[0] = p_a[0] + vec_a[0] / 2;
	m_pa[1] = p_a[1] + vec_a[1] / 2;

	m_pb[0] = p_b[0] + vec_b[0] / 2;
	m_pb[1] = p_b[1] + vec_b[1] / 2;

	m_pc[0] = p_c[0] + vec_c[0] / 2;
	m_pc[1] = p_c[1] + vec_c[1] / 2;



	return (v_a * v_b * v_c) / (4 * A);
}

float Triangle::Areal() {
	float v_a = points[0]->returndistance(*points[1]);
	float v_b = points[1]->returndistance(*points[2]);
	float v_c = points[2]->returndistance(*points[0]);

	float s = (v_a + v_b + v_c) / 2;

	return (sqrt)(s * (s - v_a) * (s - v_b) * (s - v_c));
}