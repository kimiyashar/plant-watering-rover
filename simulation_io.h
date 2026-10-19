#pragma once

#include "rover_controller.h"
#include <istream>
#include <string>
#include <vector>

std::vector<CameraFrame> loadCameraFrames(std::istream& input);
std::vector<CameraFrame> loadCameraFramesFile(const std::string& path);
