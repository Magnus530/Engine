#include "epch.h"
#include "Readers.h"
#include "Terrain.h"

Terrain::Terrain() {

}

Terrain::Terrain(std::string filepath) {
	char format_arr[4];
	
	int file_length = filepath.length();
	for (int i = 3; i >= 0; i--) {
		format_arr[i] = filepath[file_length];
		file_length--;
	}
	
	std::string format = format_arr;
	
	if (format_arr == ".las") {
		LAS_read(filepath, vertices, colors,  uv);
	}
}

Terrain::~Terrain() {

}

