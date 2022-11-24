#include "epch.h"
#include "Point.h"

void Point::addneightbor(Point n) { 
	Neightbors.push_back(n._id); 
	float cal, cal2, val1, val2; 
	val1 = info.getX() - n.info.getX();
	val2 = info.getZ() - n.info.getZ();
	cal = (sqrt)((val1 * val1) + (val2 * val2));
	val1 = cal;
	val2 = info.getY() - n.info.getY();
	cal2 = (sqrt)((val1 * val1) - (val2 * val2));
	distances.push_back({ cal, cal2 });
}


void Point::removeneighbor(Point n) {
	//Neightbors.remove(Neightbors.begin(), Neightbors.end(), n._id);
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
		//Neighbors.erase(loc);
		//distances.erase(loc);
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