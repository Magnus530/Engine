#include "epch.h"
#include "Point.h"

void Point::addneightbor(Point n) { 
	for (int i = 0; i < Neightbors.size(); i++) {
		if (Neightbors[i] == n._id) {
			return;
		}
	}
	Neightbors.push_back(n._id); 
	float cal, cal2, val1, val2; 
	val1 = getx() - n.getx();
	val2 = getz() - n.getz();
	cal = (sqrt)((val1 * val1) + (val2 * val2));
	val1 = cal;
	val2 = gety() - n.gety();
	cal2 = (sqrt)((val1 * val1) - (val2 * val2));
	distances.push_back({ cal, cal2 });
}

void Point::removeneighbor(Point n) {
	bool found{ false };
	int loc{ 0 };
	while (!found) {
		if (Neightbors[loc] == n._id) {
			found = !found;
			break;
		}
		loc++;
	}
	if (found) {
		int last = Neightbors.size() - 1;
		int temp;
		float temp_k, temp_h;
		temp = Neightbors[loc];
		temp_k = distances[loc][0];
		temp_h = distances[loc][1];
		Neightbors[loc] = Neightbors[last];
		distances[loc][0] = distances[last][0];
		distances[loc][1] = distances[last][1];
		Neightbors[last] = temp;
		distances[last][0] = temp_k;
		distances[last][1] = temp_h;
		Neightbors.pop_back();
		distances.pop_back();
	}
}

float Point::returndistance(Point n) {
	int index{ 0 };
	for (int i = 0; i < Neightbors.size(); i++) {
		if (n._id == Neightbors[i]) {
			index = i;
			break;
		}
	}
	return distances[index][0];
}