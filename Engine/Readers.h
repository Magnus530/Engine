#pragma once
#include <string>
#include <vector>

#include "epch.h"

void LAS_read(std::string filepath, std::vector <std::vector <float[3]>> &xyz, std::vector <std::vector <float[3]>> &rgb, std::vector <std::vector <float[2]>> &uv);