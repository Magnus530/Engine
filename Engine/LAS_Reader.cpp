#include <string>
#include <fstream>
#include <iostream>
 
#include "epch.h"
#include "Readers.h"
/*
void reduce_points(std::vector <float[3]> &p, std::vector <std::vector<float[3]>> &v, std::vector <std::vector <float[3]>>& c);

void LAS_read(std::string filepath, std::vector <std::vector <float[3]>>& xyz, std::vector <std::vector <float[3]>>& rgb, std::vector <std::vector <float[2]>>& uv) {
	float x{ 0 }, y{ 0 }, z{ 0 };
	std::ifstream inn;
	inn.open(filepath.c_str());
	
	std::vector <float[3]> Points;

	if (inn.is_open()) {
		int n;
		inn >> n;
		for (int i = 0; i < n; i++) {
			inn >> x;
			inn >> y;
			inn >> z;

			Points.push_back({ x, z, y });
		}
		inn.close();
	}

	reduce_points(Points, xyz, rgb);
}
void reduce_points(std::vector <float[3]>& p, std::vector <std::vector<float[3]>>& v, std::vector <std::vector <float[3]>>& c) {
	float xMax = p[0][0], xMin = p[0][0], zMax = p[0][2], zMin = p[0][2];
	for (int i = 0; i < p.size(); i++) {
		if (p[i][0] < xMin) {
			xMin = p[i][0];
		}
		if (p[i][0] > xMax) {
			xMax = p[i][0];
		}
		if (p[i][2] < zMin) {
			zMin = p[i][2];
		}
		if (p[i][2] > zMax) {
			zMax = p[i][2];
		}
	}

	const float ekvidistanse{ 1 };	

	float length = xMax - xMin;
	float width = zMax - zMin;

	float h{ 0 };
	float count{ 0 };

	for (float x = ((-1)*length/2); x < length/2; x+= ekvidistanse){
		std::vector <float[3]> temp;
		for (float z = ((-1) * width / 2); z < width / 2; z++) {
			for (int i = 0; i < p.size(); i++) {
				if ((p[i][0] >= x && p[i][0] <= x + ekvidistanse) && (p[i][2] >= z && p[i][2] <= z + ekvidistanse)) {
					h += p[i][1];
					count++;
				}
			}
			if (count > 0)
			{
				temp.push_back({ x, h / count, z });
				count = 0;
			}
			else {
				temp.push_back({ x, 0, z });
			}
		}
		v.push_back(temp);
	}
}*/